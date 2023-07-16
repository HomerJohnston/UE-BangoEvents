#include "Puny/PunyEvent.h"

#include "Puny/PunyAction.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

UPunyEvent::UPunyEvent()
{
}

void UPunyEvent::RegisterAction(UPunyAction* Action)
{
	ActionTrigger.AddDynamic(Action, &UPunyAction::HandleSignal);
}

void UPunyEvent::UnregisterAction(UPunyAction* Action)
{
	ActionTrigger.RemoveDynamic(Action, &UPunyAction::HandleSignal);
}

void UPunyEvent::RespondToTrigger(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
}
