#include "Puny/PunyEvent_Bang.h"

#include "Bango/Utility/Log.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

void UPunyEvent_Bang::RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	EPunyEventSignalType ActionSignal;

	switch (Signal.Type)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{
			ActionSignal = EPunyEventSignalType::StartAction;
			break;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			UE_LOG(Bango, Warning, TEXT("UPunyEvent_Bang ignoring Deactivate trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
			ActionSignal = EPunyEventSignalType::None;
			break;
		}
		default:
		{
			ActionSignal = EPunyEventSignalType::None;
			break;
		}
	}
	
	EventSignal.Broadcast(this, FPunyEventSignal(ActionSignal, Signal.Instigator));
}
