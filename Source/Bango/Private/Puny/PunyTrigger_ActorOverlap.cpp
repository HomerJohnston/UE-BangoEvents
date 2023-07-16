#include "Puny/PunyTrigger_ActorOverlap.h"

#include "Puny/PunyEventComponent.h"

void UPunyTrigger_ActorOverlap::Enable_Implementation()
{
	AActor* TargetActor = GetActor();

	TargetActor->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnActorBeginOverlap);
	TargetActor->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnActorEndOverlap);
}

void UPunyTrigger_ActorOverlap::Disable_Implementation()
{
	AActor* TargetActor = GetActor();

	TargetActor->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnActorBeginOverlap);
	TargetActor->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::OnActorEndOverlap);
}

void UPunyTrigger_ActorOverlap::OnActorBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	SendSignal(FPunyTriggerSignal(OnBeginOverlap, InstigatorActor));
}

void UPunyTrigger_ActorOverlap::OnActorEndOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	SendSignal(FPunyTriggerSignal(OnEndOverlap, InstigatorActor));
}
