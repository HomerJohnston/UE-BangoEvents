// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/DefaultImpl/OverlapTriggerConditions.h"

#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoInstigatorFilter.h"



void UBangoTriggerCondition_OnBeginOverlap::Enable_Implementation()
{
	GetEvent()->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

void UBangoTriggerCondition_OnBeginOverlap::Disable_Implementation()
{
	GetEvent()->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnBeginOverlap);
}

void UBangoTriggerCondition_OnBeginOverlap::OnBeginOverlap(AActor* BangoEventActor, AActor* InstigatorActor)
{
	ABangoEvent* BangoEvent = Cast<ABangoEvent>(BangoEventActor);

	check(BangoEvent);
	
	if (IsValid(ActorFilter))
	{
		if (!ActorFilter->IsValidInstigator(BangoEvent, InstigatorActor))
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
