﻿#include "Puny/Core/Event_Toggle.h"

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/Log.h"
#include "Puny/Core/EventSignal.h"
#include "Puny/Core/TriggerSignal.h"

UPunyEvent_Toggle::UPunyEvent_Toggle()
{
	ToggleState = EPunyEvent_ToggleState::Deactivated;
}

bool UPunyEvent_Toggle::GetIsExpired()
{
	if (ToggleState == EPunyEvent_ToggleState::Activated)
	{
		return false;
	}
	
	if (!GetUsesActivateLimit())
	{
		return false;
	}

	return GetActivateCount() >= GetActivateLimit();
}

void UPunyEvent_Toggle::Init()
{
	Super::Init();
	
	if (bStartActivated)
	{
		Activate(GetActor());
	}
}

EPunyEventSignalType UPunyEvent_Toggle::RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{	
	UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle receiving signal: %s from %s"), *StaticEnum<EPunyTriggerSignalType>()->GetValueAsString(Signal.Type), *Signal.Instigator->GetName());
	
	switch (Signal.Type)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{			
			return (Activate(Signal.Instigator)) ? EPunyEventSignalType::StartAction : EPunyEventSignalType::None;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			return (Deactivate(Signal.Instigator)) ? EPunyEventSignalType::StopAction : EPunyEventSignalType::None;
		}
		default:
		{
			UE_LOG(Bango, Error, TEXT("UPunyEvent_Toggle ignoring Unknown trigger from <%s>"), *Signal.Instigator->GetName());
			return EPunyEventSignalType::None;
		}
	}
}

bool UPunyEvent_Toggle::Activate(UObject* Instigator)
{	
	if (GetUsesActivateLimit() && GetActivateCount() >= GetActivateLimit())
	{
		return false;
	}
	
	bool bSetToggleState = ToggleState != EPunyEvent_ToggleState::Activated;

	if (bSetToggleState)
	{
		if (SetToggleState(EPunyEvent_ToggleState::Activated))
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

bool UPunyEvent_Toggle::Deactivate(UObject* Instigator)
{
	bool bSetToggleState;

	switch (DeactivateCondition)
	{
		case EPunyEvent_ToggleDeactivateCondition::AnyDeactivateTrigger:
		{
			bSetToggleState = true;
			break;
		}
		case EPunyEvent_ToggleDeactivateCondition::AnyInstigatorRemoved:
		{
			bSetToggleState = InstigatorRecords.IsInstigatorActive(Instigator);
			break;
		}
		case EPunyEvent_ToggleDeactivateCondition::AllInstigatorsRemoved:
		{
			bSetToggleState = InstigatorRecords.GetNumActiveInstigators() == 1 && InstigatorRecords.IsInstigatorActive(Instigator);
			break;
		}
		case EPunyEvent_ToggleDeactivateCondition::OriginalInstigatorRemoved:
		{
			bSetToggleState = Instigator == InstigatorRecords.GetFirstInstigator();
			break;
		}
		default:
		{
			UE_LOG(Bango, Error, TEXT("UPunyEvent_Toggle has invalid deactivate condition!"));
			bSetToggleState = false;
		}
	}

	if (bSetToggleState)
	{
		if (SetToggleState(EPunyEvent_ToggleState::Deactivated))
		{
			UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle <%s>: Deactivated"), *GetName());
			InstigatorRecords.ClearActiveInstigators();
		}
		else
		{
			UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Toggle <%s>: Failed to deactivate!"), *GetName());
		}
	}

	InstigatorRecords.UpdateInstigatorRecord(Instigator, EPunyEventSignalType::StopAction, GetWorld()->GetTimeSeconds());

	UE_LOG(Bango, VeryVerbose, TEXT("Event now has %i active instigators"), InstigatorRecords.GetNumActiveInstigators());
	return true;
}

bool UPunyEvent_Toggle::SetToggleState(EPunyEvent_ToggleState NewState)
{
	if (ToggleState == NewState)
	{
		return false;
	}
	
	ToggleState = NewState;

	return true;
}

FLinearColor UPunyEvent_Toggle::GetDisplayBaseColor()
{
	return BangoColor::GreenBase;
}

void UPunyEvent_Toggle::ApplyColorEffects(FLinearColor& Color)
{
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}
	
	if (ToggleState == EPunyEvent_ToggleState::Activated)
	{
		Color = BangoColorOps::BrightenColor(Color);
	}
}

bool UPunyEvent_Toggle::GetIsPlungerPushed()
{
	return ToggleState == EPunyEvent_ToggleState::Activated;
}

void UPunyEvent_Toggle::AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data)
{
	Super::AppendDebugDataString_Game(Data);

	for (TTuple<UObject*, FPunyInstigatorRecord>& KVP : InstigatorRecords.GetData())
	{
		UObject* Instigator = KVP.Key;

		if (!InstigatorRecords.IsInstigatorActive(KVP.Key))
		{
			return;
		}

		Data.Add(FBangoDebugTextEntry("Instigator:", FString::Printf(TEXT("%s"), *Instigator->GetName())));	
	}
}