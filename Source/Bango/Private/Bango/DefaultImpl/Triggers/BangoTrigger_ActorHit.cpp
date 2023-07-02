#include "Bango/DefaultImpl/Triggers/BangoTrigger_ActorHit.h"

#include "Bango/DefaultImpl/Triggers/BangoTriggerInstigatorAction.h"
#include "Bango/Core/BangoInstigatorFilter.h"

UBangoTrigger_ActorHit::UBangoTrigger_ActorHit()
{
	TriggerAction = EBangoTriggerInstigatorAction::AddInstigator;
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
	if (IsValid(InstigatorFilter))
	{
		if (!InstigatorFilter->IsValidInstigator(HitActor, InstigatorActor))
		{
			return;
		}
	}

	switch (TriggerAction)
	{
		case EBangoTriggerInstigatorAction::AddInstigator:
			ActivateEvent(InstigatorActor);
			break;
		case EBangoTriggerInstigatorAction::RemoveInstigator:
			DeactivateEvent(InstigatorActor);
			break;
	}
}

