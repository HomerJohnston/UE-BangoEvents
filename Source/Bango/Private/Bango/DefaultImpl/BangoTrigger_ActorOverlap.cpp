// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/DefaultImpl/BangoTrigger_ActorOverlap.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoInstigatorFilter.h"



// ============================================================================================
void UBangoTrigger_ActorOverlap::Enable_Implementation()
{
	AActor* EventActor = GetEvent();

	EventActor->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	EventActor->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	SubscribedActor = EventActor;
}

void UBangoTrigger_ActorOverlap::Disable_Implementation()
{
	if (!SubscribedActor.IsValid())
	{
		return;
	}

	// TODO: I should be able to handle changing the target actor during runtime, or else prevent it.
	AActor* Actor = GetEvent();

	if (SubscribedActor != Actor)
	{
		UE_LOG(Bango, Warning, TEXT("Warning, subscribed actor does not match target actor!"));
	}
	
	SubscribedActor->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnBeginOverlap);
	SubscribedActor->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::OnEndOverlap);

	SubscribedActor = nullptr;
}

void UBangoTrigger_ActorOverlap::OnBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	if (IsValid(ActorFilter))
	{
		if (!ActorFilter->IsValidInstigator(OverlapActor, InstigatorActor))
		{
			return;
		}
	}

	ActivateEvent(InstigatorActor);
}

void UBangoTrigger_ActorOverlap::OnEndOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	if (IsValid(ActorFilter))
	{
		if (!ActorFilter->IsValidInstigator(OverlapActor, InstigatorActor))
		{
			return;
		}
	}

	DeactivateEvent(InstigatorActor);
}


