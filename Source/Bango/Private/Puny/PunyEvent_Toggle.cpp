#include "Puny/PunyEvent_Toggle.h"

#include "Bango/Utility/Log.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

UPunyEvent_Toggle::UPunyEvent_Toggle()
{
	ToggleState = EPunyEvent_ToggleState::Deactivated;
}

void UPunyEvent_Toggle::Init()
{
	if (bStartActivated)
	{
		SetToggleState(EPunyEvent_ToggleState::Activated);
	}
}

void UPunyEvent_Toggle::RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	UE_LOG(Bango, Display, TEXT("UBangoEvent_Toggle receiving signal: %s from %s"), *StaticEnum<EPunyTriggerSignalType>()->GetValueAsString(Signal.Type), *Signal.Instigator->GetName());
	
	switch (Signal.Type)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{
			Activate(Signal.Instigator);
			break;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			Deactivate(Signal.Instigator);
			break;
		}
		default:
		{
			break;
		}
	}
}

void UPunyEvent_Toggle::Activate(UObject* Instigator)
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
			
			break;
		}
		case EPunyEvent_ToggleDeactivateCondition::AllInstigatorsRemoved:
		{
			break;
		}
		case EPunyEvent_ToggleDeactivateCondition::OriginalInstigatorRemoved:
		{
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
		if (SetToggleState(EPunyEvent_ToggleState::Activated))
		{
			UE_LOG(Bango, Display, TEXT("UBangoEvent_Toggle <%s>: Activated"), *GetName());
			EventSignal.Broadcast(this, FPunyEventSignal(EPunyEventSignalType::StartAction, Instigator));
		}
	}
	else
	{
		UE_LOG(Bango, Display, TEXT("UBangoEvent_Toggle <%s>: Failed to activate!"), *GetName());
	}
}

void UPunyEvent_Toggle::Deactivate(UObject* Instigator)
{
	if (SetToggleState(EPunyEvent_ToggleState::Deactivated))
	{
		UE_LOG(Bango, Display, TEXT("UBangoEvent_Toggle <%s>: Deactivated"), *GetName());
		EventSignal.Broadcast(this, FPunyEventSignal(EPunyEventSignalType::StopAction, Instigator));
	}
	else
	{
		UE_LOG(Bango, Display, TEXT("UBangoEvent_Toggle <%s>: Failed to deactivate!"), *GetName());
	}
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
