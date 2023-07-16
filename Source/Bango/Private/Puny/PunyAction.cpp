#include "Puny/PunyAction.h"

#include "Puny/PunyEventSignal.h"
#include "Puny/PunyEventComponent.h"

void UPunyAction::HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal)
{
	checkNoEntry();
}

UWorld* UPunyAction::GetWorld() const
{
	return UObject::GetWorld();
}

UPunyEventComponent* UPunyAction::GetEvent() const
{
	return Cast<UPunyEventComponent>(GetOuter());
}

AActor* UPunyAction::GetActor() const
{
	return GetEvent()->GetOwner();
}
