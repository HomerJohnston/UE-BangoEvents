#include "Bango/Event/BangoEvent_Toggle.h"

#include "Bango/Utility/Log.h"
#include "Bango/Action/BangoAction.h"
#include "Bango/Trigger/BangoTrigger.h"
#include "Bango/Utility/BangoColorOps.h"

ABangoEvent_Toggle::ABangoEvent_Toggle()
{
	DeactivateCondition = EBangoToggleDeactivateCondition::AllInstigatorsRemoved;
}

EBangoToggleDeactivateCondition ABangoEvent_Toggle::GetDeactivateCondition() const
{
	return DeactivateCondition;
}

EBangoToggleState ABangoEvent_Toggle::GetToggleState()
{
	return ToggleState;
}

bool ABangoEvent_Toggle::SetToggleState(EBangoToggleState NewState, UObject* ByInstigator)
{
	if (ToggleState == NewState) { return false; }

	ToggleState = NewState;
	
	switch (ToggleState)
	{
		case EBangoToggleState::Activated:
		{
			SignalActions(EBangoSignal::Activate, ByInstigator);
			return true;
		}
		case EBangoToggleState::Deactivated:
		{
			SignalActions(EBangoSignal::Deactivate, ByInstigator);
			return true;
		}
		default:
		{
			return false;
		}
	}

}

bool ABangoEvent_Toggle::ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
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

bool ABangoEvent_Toggle::HasInvalidData() const
{
	return Super::HasInvalidData();
}

bool ABangoEvent_Toggle::Activate(UObject* ActivateInstigator)
{
	FBangoEventInstigationArray& ActivateInstigations = InstigatorData.FindOrAdd(EBangoSignal::Activate);

	FBangoEventInstigation* InstigationData = ActivateInstigations.Array.FindByPredicate([ActivateInstigator](FBangoEventInstigation& ArrayElement) { return ArrayElement.Equals(ActivateInstigator); } );

	if (InstigationData)
	{
		InstigationData->Time = GetWorld()->GetTimeSeconds();
		return false;
	}

	int32 InstigatorIndex = ActivateInstigations.Array.Emplace(ActivateInstigator, GetWorld()->GetTimeSeconds());

	if (InstigatorIndex == 0)
	{
		return SetToggleState(EBangoToggleState::Activated, ActivateInstigator);
	}

	return false;
}

bool ABangoEvent_Toggle::Deactivate(UObject* DeactivateInstigator)
{
	bool bDoDeactivate;
	
	FBangoEventInstigationArray* DeactivateInstigations = InstigatorData.Find(EBangoSignal::Activate);

	if (!DeactivateInstigations)
	{
		return false;
	}

	int32 Index = DeactivateInstigations->Array.IndexOfByPredicate([DeactivateInstigator](const FBangoEventInstigation& ArrayElement) { return ArrayElement.Equals(DeactivateInstigator); });
	
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
	
	if (Index != INDEX_NONE)
	{
		DeactivateInstigations->Array.RemoveAt(Index);
	}
	
	if (bDoDeactivate)
	{
		return SetToggleState(EBangoToggleState::Deactivated, DeactivateInstigator);
	}

	return false;
}

void ABangoEvent_Toggle::SignalActions(EBangoSignal Signal, UObject* StartInstigator)
{
	for (UBangoAction* Action : Actions)
	{
		Action->ReceiveEventSignal(Signal, StartInstigator);
	}
}

void ABangoEvent_Toggle::ResetRemainingTriggerLimits()
{
	RemainingTriggerLimits = { EBangoSignal::Activate, EBangoSignal::Deactivate };
}

void ABangoEvent_Toggle::SetFrozen(bool bFreeze)
{
	if (bFreeze)
	{
		if (GetToggleState() == EBangoToggleState::Activated)
		{
			OnEventTriggered.AddDynamic(this, &ThisClass::PerformPendingFreeze);
		}
		else
		{
			Super::SetFrozen(bFreeze);
		}	
	}
	else
	{
		OnEventTriggered.RemoveDynamic(this, &ThisClass::PerformPendingFreeze);
		Super::SetFrozen(bFreeze);
	}
}

void ABangoEvent_Toggle::PerformPendingFreeze(ABangoEvent* Event, EBangoSignal Signal, UObject* NewInstigator)
{
	if (Signal == EBangoSignal::Deactivate)
	{
		Super::SetFrozen(true);
	}
}

// ================================================================================================
// EDITOR
// ================================================================================================

#if WITH_EDITOR
void ABangoEvent_Toggle::UpdateProxyState()
{
	Super::UpdateProxyState();

	if (GetWorld()->IsGameWorld())
	{
		double LastActivateTime = GetLastTriggerTime(EBangoSignal::Activate);
		double LastDeactivateTime = GetLastTriggerTime(EBangoSignal::Deactivate);
		
		CurrentState.SetFlag(EBangoEventState::Active, LastActivateTime > LastDeactivateTime);

		UE_LOG(Bango, Display, TEXT("Setting CurrentState, new value %i"), CurrentState.Value);
	}
}
#endif

#if WITH_EDITOR
FLinearColor ABangoEvent_Toggle::GetColorBase() const
{
	return BangoColors::GreenBase;
}
#endif

#if WITH_EDITOR
FLinearColor ABangoEvent_Toggle::GetColorForProxy() const
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
#endif

#if WITH_EDITOR
TArray<FBangoDebugTextEntry> ABangoEvent_Toggle::GetDebugDataString_Game() const
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
#endif

#if WITH_EDITOR
TArray<FBangoDebugTextEntry> ABangoEvent_Toggle::GetDebugDataString_Editor() const
{
	return Super::GetDebugDataString_Editor();
}
#endif
