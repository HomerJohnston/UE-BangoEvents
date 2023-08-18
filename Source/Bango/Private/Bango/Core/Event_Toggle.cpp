#include "Bango/Core/BangoEvent_Toggle.h"

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/BangoLog.h"
#include "Bango/Core/BangoTriggerSignal.h"

UBangoEvent_Toggle::UBangoEvent_Toggle()
{
	ToggleState = EBangoEvent_ToggleState::Deactivated;
}

bool UBangoEvent_Toggle::GetIsExpired()
{
	if (ToggleState == EBangoEvent_ToggleState::Activated)
	{
		return false;
	}
	
	if (!GetUsesActivateLimit())
	{
		return false;
	}

	return GetActivateCount() >= GetActivateLimit();
}

void UBangoEvent_Toggle::Init()
{
	Super::Init();
	
	if (bStartActivated)
	{
		Activate(GetActor());
	}
}

EBangoEventSignalType UBangoEvent_Toggle::RespondToTriggerSignal_Impl(UBangoTrigger* Trigger, FBangoTriggerSignal Signal)
{	
	UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle receiving signal: %s from %s"), *StaticEnum<EBangoTriggerSignalType>()->GetValueAsString(Signal.Type), *Signal.Instigator->GetName());
	
	switch (Signal.Type)
	{
		case EBangoTriggerSignalType::ActivateEvent:
		{			
			return (Activate(Signal.Instigator)) ? EBangoEventSignalType::EventActivated : EBangoEventSignalType::None;
		}
		case EBangoTriggerSignalType::DeactivateEvent:
		{
			return (Deactivate(Signal.Instigator)) ? EBangoEventSignalType::EventDeactivated : EBangoEventSignalType::None;
		}
		default:
		{
			UE_LOG(Bango, Error, TEXT("UBangoEvent_Toggle ignoring Unknown trigger from <%s>"), *Signal.Instigator->GetName());
			return EBangoEventSignalType::None;
		}
	}
}

bool UBangoEvent_Toggle::Activate(UObject* Instigator)
{	
	if (GetUsesActivateLimit() && GetActivateCount() >= GetActivateLimit())
	{
		return false;
	}
	
	bool bSetToggleState = ToggleState != EBangoEvent_ToggleState::Activated;

	if (bSetToggleState)
	{
		if (SetToggleState(EBangoEvent_ToggleState::Activated))
		{
			UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle <%s>: Activated"), *GetName());
		}
		else
		{
			UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle <%s>: Failed to activate!"), *GetName());
		}	
	}
	
	UE_LOG(Bango, VeryVerbose, TEXT("Event now has %i active instigators"), InstigatorRecords.GetNumActiveInstigators());
	return true;
}

bool UBangoEvent_Toggle::Deactivate(UObject* Instigator)
{
	bool bSetToggleState;

	switch (DeactivateCondition)
	{
		case EBangoEvent_ToggleDeactivateCondition::AnyDeactivateTrigger:
		{
			bSetToggleState = true;
			break;
		}
		case EBangoEvent_ToggleDeactivateCondition::AnyInstigatorRemoved:
		{
			bSetToggleState = InstigatorRecords.IsInstigatorActive(Instigator);
			break;
		}
		case EBangoEvent_ToggleDeactivateCondition::AllInstigatorsRemoved:
		{
			bSetToggleState = InstigatorRecords.GetNumActiveInstigators() == 1 && InstigatorRecords.IsInstigatorActive(Instigator);
			break;
		}
		case EBangoEvent_ToggleDeactivateCondition::OriginalInstigatorRemoved:
		{
			bSetToggleState = Instigator == InstigatorRecords.GetFirstInstigator();
			break;
		}
		default:
		{
			UE_LOG(Bango, Error, TEXT("UBangoEvent_Toggle has invalid deactivate condition!"));
			bSetToggleState = false;
		}
	}

	if (bSetToggleState)
	{
		if (SetToggleState(EBangoEvent_ToggleState::Deactivated))
		{
			UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle <%s>: Deactivated"), *GetName());
			InstigatorRecords.ClearActiveInstigators();
		}
		else
		{
			UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle <%s>: Failed to deactivate!"), *GetName());
		}
	}

	InstigatorRecords.UpdateInstigatorRecord(Instigator, EBangoEventSignalType::EventDeactivated, GetWorld()->GetTimeSeconds());

	UE_LOG(Bango, VeryVerbose, TEXT("Event now has %i active instigators"), InstigatorRecords.GetNumActiveInstigators());
	return true;
}

bool UBangoEvent_Toggle::SetToggleState(EBangoEvent_ToggleState NewState)
{
	if (ToggleState == NewState)
	{
		return false;
	}
	
	ToggleState = NewState;

	return true;
}

FLinearColor UBangoEvent_Toggle::GetDisplayBaseColor()
{
	return BangoColor::GreenBase;
}

void UBangoEvent_Toggle::ApplyColorEffects(FLinearColor& Color)
{
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}
	
	if (ToggleState == EBangoEvent_ToggleState::Activated)
	{
		Color = BangoColorOps::BrightenColor(Color);
	}
}

bool UBangoEvent_Toggle::GetIsActive()
{
	return ToggleState == EBangoEvent_ToggleState::Activated;
}

void UBangoEvent_Toggle::AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data)
{
	Super::AppendDebugDataString_Game(Data);

	for (TTuple<UObject*, FBangoInstigatorRecord>& KVP : InstigatorRecords.GetData())
	{
		UObject* Instigator = KVP.Key;

		if (!InstigatorRecords.IsInstigatorActive(KVP.Key))
		{
			return;
		}

		Data.Add(FBangoDebugTextEntry("Instigator:", FString::Printf(TEXT("%s"), *Instigator->GetName())));	
	}
}
