#include "Bango/Event/BangoToggleEvent.h"

#include "Bango/Action/BangoAction.h"
#include "Bango/Action/BangoToggleAction.h"
#include "Bango/Trigger/BangoTrigger.h"
#include "Bango/Utility/BangoColorOps.h"

ABangoToggleEvent::ABangoToggleEvent()
{
	DeactivateCondition = EBangoToggleDeactivateCondition::AllInstigatorsRemoved;
}

EBangoToggleDeactivateCondition ABangoToggleEvent::GetDeactivateCondition() const
{
	return DeactivateCondition;
}

EBangoToggleState ABangoToggleEvent::GetToggleState()
{
	return ToggleState;
}

bool ABangoToggleEvent::SetToggleState(EBangoToggleState NewState, UObject* ByInstigator)
{
	if (ToggleState == NewState) { return false; }

	ToggleState = NewState;

	switch (ToggleState)
	{
		case EBangoToggleState::Activated:
		{
			StartActions(ByInstigator);
			return true;
		}
		case EBangoToggleState::Deactivated:
		{
			StopActions(ByInstigator);
			return true;
		}
		default:
		{
			return false;
		}
	}
}

bool ABangoToggleEvent::ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	switch (Signal)
	{
		case EBangoSignal::Activate:
		{
			return Activate(NewInstigator);
		}
		case EBangoSignal::Deactivate:
		{
			return Deactivate(NewInstigator);
		}
		default:
		{
			return false;
		}
	}
}

bool ABangoToggleEvent::HasInvalidData() const
{
	return Super::HasInvalidData();
}

bool ABangoToggleEvent::Activate(UObject* ActivateInstigator)
{
	FBangoInstigationDataCtr& ActivateInstigations = InstigatorData.FindOrAdd(EBangoSignal::Activate);

	FBangoInstigationData* InstigationData = ActivateInstigations.Array.FindByPredicate([ActivateInstigator](FBangoInstigationData& ArrayElement) { return ArrayElement.Equals(ActivateInstigator); } );

	if (InstigationData)
	{
		InstigationData->Time = GetWorld()->GetTimeSeconds();
		return false;
	}

	int32 InstigatorIndex = ActivateInstigations.Array.Emplace(ActivateInstigator, GetWorld()->GetTimeSeconds());

	if (InstigatorIndex == 0)
	{
		SetToggleState(EBangoToggleState::Activated, ActivateInstigator);
		return true;
	}

	return false;
}

void ABangoToggleEvent::StartActions(UObject* StartInstigator)
{
	for (UBangoAction* Action : Actions)
	{
		Action->Start(StartInstigator);
	}
}

void ABangoToggleEvent::SetFrozen(bool bFreeze)
{
	if (bFreeze)
	{
		if (GetToggleState() == EBangoToggleState::Activated)
		{
			OnBangoEventTriggered.AddDynamic(this, &ThisClass::PerformPendingFreeze);
		}
		else
		{
			Super::SetFrozen(bFreeze);
		}	
	}
	else
	{
		OnBangoEventTriggered.RemoveDynamic(this, &ThisClass::PerformPendingFreeze);
		Super::SetFrozen(bFreeze);
	}
}

void ABangoToggleEvent::PerformPendingFreeze(ABangoEvent* Event, EBangoSignal Signal, UObject* NewInstigator)
{
	if (Signal == EBangoSignal::Deactivate)
	{
		Super::SetFrozen(true);
	}
}

bool ABangoToggleEvent::Deactivate(UObject* DeactivateInstigator)
{
	bool bDoDeactivate;
	
	FBangoInstigationDataCtr* DeactivateInstigations = InstigatorData.Find(EBangoSignal::Activate);

	if (!DeactivateInstigations)
	{
		return false;
	}

	int32 Index = DeactivateInstigations->Array.IndexOfByPredicate([DeactivateInstigator](const FBangoInstigationData& ArrayElement) { return ArrayElement.Equals(DeactivateInstigator); });
	
	switch (GetDeactivateCondition())
	{
		case EBangoToggleDeactivateCondition::AllInstigatorsRemoved:
		{
			bDoDeactivate = Index != INDEX_NONE && DeactivateInstigations->Array.Num() == 1;
			break;
		}
		case EBangoToggleDeactivateCondition::AnyInstigatorsRemoved:
		{
			bDoDeactivate = Index != INDEX_NONE;
			break;
		}
		case EBangoToggleDeactivateCondition::OriginalInstigatorRemoved:
		{
			bDoDeactivate = Index != INDEX_NONE && DeactivateInstigations->Array.Num() > 0 && DeactivateInstigations->Array[0].Instigator == DeactivateInstigator;
			break;
		}
		case EBangoToggleDeactivateCondition::AnyDeactivateRequest:
		{
			bDoDeactivate = true;
			break;
		}
		default:
		{
			bDoDeactivate = false;
			break;
		}
	}

	if (bDoDeactivate)
	{
		return SetToggleState(EBangoToggleState::Deactivated, DeactivateInstigator);
	}
	else if (Index != INDEX_NONE)
	{
		DeactivateInstigations->Array.RemoveAt(Index);
	}

	return false;
}

void ABangoToggleEvent::StopActions(UObject* StopInstigator)
{
	for (UBangoAction* Action : Actions)
	{
		UBangoToggleAction* ToggleAction = Cast<UBangoToggleAction>(Action);

		if (ToggleAction)
		{
			ToggleAction->Stop(StopInstigator);
		}
	}
}

void ABangoToggleEvent::UpdateProxyState()
{
	Super::UpdateProxyState();

	if (GetWorld()->IsGameWorld())
	{
		CurrentState.SetFlag(EBangoEventState::Active, GetLastTriggerTime(EBangoSignal::Activate) > GetLastTriggerTime(EBangoSignal::Deactivate));
	}
}

FLinearColor ABangoToggleEvent::GetColorBase() const
{
	return FColor::Green;
}

FLinearColor ABangoToggleEvent::GetColorForProxy() const
{
	FLinearColor Color = Super::GetColorForProxy();

	if (GetWorld()->IsGameWorld())
	{
		double LastHandleDownTime = GetLastTriggerTime(EBangoSignal::Activate);
		double LastHandleUpTime = GetLastTriggerTime(EBangoSignal::Deactivate);
			
		FLinearColor ActivationColor = BangoColorOps::BrightenColor(Color);
		FLinearColor DeactivationColor = BangoColorOps::VeryDarkDesatColor(Color);
			
		double ElapsedTimeSinceLastActivation = GetWorld()->GetTimeSeconds() - LastHandleDownTime;
		double ActivationAlpha = FMath::Clamp(ElapsedTimeSinceLastActivation / 0.2, 0, 1);
			
		if (IsValid(GWorld) && (ActivationAlpha > 0))
		{
			Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
		}
			
		double ElapsedTimeSinceLastDeactivation = GetWorld()->GetTimeSeconds() - LastHandleUpTime;
		double DeactivationAlpha = FMath::Clamp(ElapsedTimeSinceLastDeactivation / (2.f * 0.2), 0, 1);
			
		if (IsValid(GWorld) && (DeactivationAlpha > 0))
		{
			Color = FMath::Lerp(DeactivationColor, Color, DeactivationAlpha);
		}
	}

	return Color;
}

TArray<FBangoDebugTextEntry> ABangoToggleEvent::GetDebugDataString_Game() const
{
	TArray<FBangoDebugTextEntry> Data = Super::GetDebugDataString_Game(); 

	/*
	for(const FBangoInstigationData& InstigatorData : GetInstigators())
	{
		Data.Add(FBangoDebugTextEntry("Instigator:", FString::Printf(TEXT("%s"), *InstigatorData.Instigator->GetName())));	
	}
	*/
	
	return Data;
}

TArray<FBangoDebugTextEntry> ABangoToggleEvent::GetDebugDataString_Editor() const
{
	return Super::GetDebugDataString_Editor();
}
