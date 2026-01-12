#include "Bango/Default/Triggers/BangoTrigger_ActorHit.h"

#include "GameFramework/Actor.h"

UBangoTrigger_ActorHit::UBangoTrigger_ActorHit()
{
	SignalOnHit = EBangoTriggerSignalType::DoNothing;
}

void UBangoTrigger_ActorHit::Enable_Implementation()
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

void UBangoTrigger_ActorHit::Disable_Implementation()
{
	if (!SubscribedActor.IsValid())
	{
		return;
	}

	SubscribedActor->OnActorHit.RemoveDynamic(this, &ThisClass::OnActorHit);

	SubscribedActor = nullptr;
}

void UBangoTrigger_ActorHit::SetTargetActor(AActor* NewTargetActor)
{
	if (NewTargetActor == SubscribedActor)
	{
		return;
	}

	Disable();

	TargetActor = NewTargetActor;

	Enable();
}

void UBangoTrigger_ActorHit::OnActorHit(AActor* HitActor, AActor* InstigatorActor, FVector NormalImpulse, const FHitResult& Hit)
{
	SendSignal(FBangoTriggerSignal(SignalOnHit, InstigatorActor));
}

