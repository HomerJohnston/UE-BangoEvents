#include "Puny/PunyEvent_Toggle.h"

#include "Bango/Utility/Log.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

void UPunyEvent_Toggle::RespondToTrigger(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	UE_LOG(Bango, Display, TEXT("UBangoEvent_Toggle receiving signal: %s from %s"), *StaticEnum<EPunyTriggerSignalType>()->GetValueAsString(Signal.Type), *Signal.Instigator->GetName());
	
	switch (Signal)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{
			Activate(FPunyEventSignal(EPunyEventSignalType::StartAction, Signal.Instigator));
			break;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			Deactivate(FPunyEventSignal(EPunyEventSignalType::StopAction, Signal.Instigator));
			break;
		}
		default:
		{
			break;
		}
	}
}

void UPunyEvent_Toggle::Activate(FPunyEventSignal Signal)
{
	
}

void UPunyEvent_Toggle::Deactivate(FPunyEventSignal Signal)
{
	
}

void UPunyEvent_Toggle::SetToggleState(EPunyEvent_ToggleState NewState)
{
	ToggleState = NewState;

	ActionTrigger.Broadcast(this, FPunyEventSignal)
}
