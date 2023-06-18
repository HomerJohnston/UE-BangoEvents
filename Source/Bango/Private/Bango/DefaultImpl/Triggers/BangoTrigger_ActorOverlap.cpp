// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/DefaultImpl/Triggers/BangoTrigger_ActorOverlap.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoInstigatorFilter.h"



// ============================================================================================
void UBangoTrigger_ActorOverlap::Enable_Implementation()
{
	AActor* ActorToUse = (bUseTargetActor && IsValid(TargetActor)) ? TargetActor : GetEvent();
	
	if (SubscribedActor.IsValid())
	{
		if (SubscribedActor == ActorToUse)
		{
			return;
		}
		else
		{
			Disable();
		}
	}
	
	ActorToUse->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	ActorToUse->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	SubscribedActor = ActorToUse;
}

void UBangoTrigger_ActorOverlap::Disable_Implementation()
{
	if (!SubscribedActor.IsValid())
	{
		return;
	}
	
	SubscribedActor->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnBeginOverlap);
	SubscribedActor->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::OnEndOverlap);

	SubscribedActor = nullptr;
}

void UBangoTrigger_ActorOverlap::SetTargetActor(AActor* NewTargetActor)
{
	if (NewTargetActor == SubscribedActor)
	{
		return;
	}

	Disable();
	
	TargetActor = NewTargetActor;
	bUseTargetActor = true;

	Enable();
}

void UBangoTrigger_ActorOverlap::OnBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	if (IsValid(InstigatorFilter))
	{
		if (!InstigatorFilter->IsValidInstigator(OverlapActor, InstigatorActor))
		{
			return;
		}
	}

	ActivateEvent(InstigatorActor);
}

void UBangoTrigger_ActorOverlap::OnEndOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	if (IsValid(InstigatorFilter))
	{
		if (!InstigatorFilter->IsValidInstigator(OverlapActor, InstigatorActor))
		{
			return;
		}
	}

	DeactivateEvent(InstigatorActor);
}


