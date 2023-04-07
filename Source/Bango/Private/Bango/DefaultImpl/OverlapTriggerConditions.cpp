// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/DefaultImpl/OverlapTriggerConditions.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoInstigatorFilter.h"



void UBangoTriggerCondition_OnBeginOverlap::Enable_Implementation()
{
	if (bUseTargetActor && !IsValid(TargetActor))
	{
		UE_LOG(Bango, Warning, TEXT("Trigger <%s> on event <%s> is set to use a target actor, but target isn't valid"), *GetName(), *GetEvent()->GetName());
		return;
	}

	AActor* Actor = bUseTargetActor ? TargetActor : GetEvent();

	Actor->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);

	SubscribedActor = Actor;
}

void UBangoTriggerCondition_OnBeginOverlap::Disable_Implementation()
{
	if (SubscribedActor.IsValid())
	{
		SubscribedActor->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnBeginOverlap);
	}

	SubscribedActor = nullptr;
}

void UBangoTriggerCondition_OnBeginOverlap::OnBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	if (IsValid(ActorFilter))
	{
		if (!ActorFilter->IsValidInstigator(OverlapActor, InstigatorActor))
		{
			return;
		}
	}
	
	OnTrigger.Execute(InstigatorActor);
}





void UBangoTriggerCondition_OnEndOverlap::Enable_Implementation()
{
	GetEvent()->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void UBangoTriggerCondition_OnEndOverlap::Disable_Implementation()
{
	GetEvent()->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnEndOverlap);
}

void UBangoTriggerCondition_OnEndOverlap::OnEndOverlap(AActor* BangoEventActor, AActor* InstigatorActor)
{
	ABangoEvent* BangoEvent = Cast<ABangoEvent>(BangoEventActor);

	check(BangoEvent);
	
	OnTrigger.Execute(InstigatorActor);
}
