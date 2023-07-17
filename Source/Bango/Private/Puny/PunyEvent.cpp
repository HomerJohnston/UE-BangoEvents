#include "Puny/PunyEvent.h"

#include "Puny/PunyAction.h"
#include "Puny/PunyEventComponent.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

UPunyEvent::UPunyEvent()
{
}

void UPunyEvent::Init()
{
}

void UPunyEvent::RegisterAction(UPunyAction* Action)
{
	EventSignal.AddDynamic(Action, &UPunyAction::HandleSignal);
}

void UPunyEvent::UnregisterAction(UPunyAction* Action)
{
	EventSignal.RemoveDynamic(Action, &UPunyAction::HandleSignal);
}

void UPunyEvent::RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
}

void UPunyEvent::AddInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType)
{
	InstigatorRecords.UpdateInstigatorRecord(Instigator, SignalType, GetWorld()->GetTimeSeconds());
}

UPunyEventComponent* UPunyEvent::GetEventComponent()
{
	return Cast<UPunyEventComponent>(GetOuter());
}

AActor* UPunyEvent::GetActor()
{
	return GetEventComponent()->GetOwner();
}
