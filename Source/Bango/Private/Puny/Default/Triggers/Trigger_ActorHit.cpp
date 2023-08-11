#include "Puny/Default/Triggers/Trigger_ActorHit.h"

#include "Puny/Core/InstigatorFilter.h"

UPunyTrigger_ActorHit::UPunyTrigger_ActorHit()
{
	SignalOnHit = EPunyTriggerSignalType::None;
}

void UPunyTrigger_ActorHit::Enable_Implementation()
{
	if (SubscribedActor.IsValid())
	{
		if (SubscribedActor == TargetActor)
		{
			return;
		}
		else
		{
			Disable();
		}
	}

	TargetActor->OnActorHit.AddDynamic(this, &ThisClass::OnActorHit);

	SubscribedActor = TargetActor;
}

void UPunyTrigger_ActorHit::Disable_Implementation()
{
	if (!SubscribedActor.IsValid())
	{
		return;
	}

	SubscribedActor->OnActorHit.RemoveDynamic(this, &ThisClass::OnActorHit);

	SubscribedActor = nullptr;
}

void UPunyTrigger_ActorHit::SetTargetActor(AActor* NewTargetActor)
{
	if (NewTargetActor == SubscribedActor)
	{
		return;
	}

	Disable();

	TargetActor = NewTargetActor;

	Enable();
}

void UPunyTrigger_ActorHit::OnActorHit(AActor* HitActor, AActor* InstigatorActor, FVector NormalImpulse, const FHitResult& Hit)
{
	SendSignal(FPunyTriggerSignal(SignalOnHit, InstigatorActor));
}

