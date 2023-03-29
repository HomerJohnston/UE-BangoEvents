#include "Bango/DefaultImpl/OverlapTriggerConditions.h"

#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoInstigatorFilter.h"



void UBangoTriggerCondition_OnBeginOverlap::Setup_Implementation(ABangoEvent* Event)
{
	Event->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

void UBangoTriggerCondition_OnBeginOverlap::Freeze_Implementation(ABangoEvent* Event)
{
	Event->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnBeginOverlap);
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





void UBangoTriggerCondition_OnEndOverlap::Setup_Implementation(ABangoEvent* Event)
{
	Event->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void UBangoTriggerCondition_OnEndOverlap::Freeze_Implementation(ABangoEvent* Event)
{
	Event->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnEndOverlap);
}

void UBangoTriggerCondition_OnEndOverlap::OnEndOverlap(AActor* BangoEventActor, AActor* InstigatorActor)
{
	ABangoEvent* BangoEvent = Cast<ABangoEvent>(BangoEventActor);

	check(BangoEvent);
	
	OnTrigger.Execute(InstigatorActor);
}
