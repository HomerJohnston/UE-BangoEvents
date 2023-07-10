// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/DefaultImpl/Triggers/BangoTrigger_ActorOverlap.h"

#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Core/BangoInstigatorFilter.h"
#include "Bango/Core/BangoSignal.h"

UBangoTrigger_ActorOverlap::UBangoTrigger_ActorOverlap()
{
	SignalOnBeginOverlap = EBangoSignal::None;
	SignalOnEndOverlap = EBangoSignal::None;
}

// ============================================================================================
void UBangoTrigger_ActorOverlap::Enable_Implementation()
{
	if (bUseTargetActor && !IsValid(TargetActor))
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use target actor but no target actor was set!"));
	}
	
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
	Handle(OverlapActor, InstigatorActor, SignalOnBeginOverlap);
}

void UBangoTrigger_ActorOverlap::OnEndOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	Handle(OverlapActor, InstigatorActor, SignalOnEndOverlap);
}

void UBangoTrigger_ActorOverlap::Handle(AActor* OverlapActor, AActor* InstigatorActor, EBangoSignal Signal)
{
	if (IsValid(InstigatorFilter))
	{
		if (!InstigatorFilter->IsValidInstigator(OverlapActor, InstigatorActor))
		{
			return;
		}
	}

	SendTriggerSignal(Signal, InstigatorActor);
}


