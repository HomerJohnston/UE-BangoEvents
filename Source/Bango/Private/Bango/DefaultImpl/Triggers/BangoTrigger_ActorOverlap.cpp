// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/DefaultImpl/Triggers/BangoTrigger_ActorOverlap.h"

#include "Bango/Utility/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/DefaultImpl/InstigatorFilters/BangoInstigatorFilter.h"
#include "Bango/Core/BangoSignal.h"

UBangoTrigger_ActorOverlap::UBangoTrigger_ActorOverlap()
{
	SignalOnBeginOverlap = EBangoSignal::None;
	SignalOnEndOverlap = EBangoSignal::None;
}

// ============================================================================================
void UBangoTrigger_ActorOverlap::Enable_Implementation()
{
	if (bUseSpecificComponent && bUseTargetActor)
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use both a component and a target actor, can only be one or the other!"));
		return;
	}

	if (bUseSpecificComponent && !IsValid(Component.GetComponent(GetEvent())))
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use a component but no component was set!"));
		return;
	}
	
	if (bUseTargetActor && !IsValid(TargetActor))
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use target actor but no target actor was set!"));
		return;
	}

	if (bUseSpecificComponent)
	{
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component.GetComponent(GetEvent()));

		if (IsValid(PrimitiveComponent))
		{
			PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
			PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

			SubscribedComponent = PrimitiveComponent;
		}

		return;
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
	if (bUseSpecificComponent)
	{
		if (!SubscribedComponent.IsValid())
		{
			return;
		}
		
		SubscribedComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnComponentBeginOverlap);
		SubscribedComponent->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnComponentEndOverlap);

		SubscribedComponent = nullptr;
		
		return;
	}
	
	if (!SubscribedActor.IsValid())
	{
		return;
	}
	
	SubscribedActor->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnBeginOverlap);
	SubscribedActor->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::OnEndOverlap);

	SubscribedActor = nullptr;
}

// TODO implement SetSpecificComponent

void UBangoTrigger_ActorOverlap::SetTargetActor(AActor* NewTargetActor)
{
	if (NewTargetActor == SubscribedActor)
	{
		return;
	}

	Disable();
	
	TargetActor = NewTargetActor;
	bUseTargetActor = true;
	bUseSpecificComponent = false;

	Enable();
}

void UBangoTrigger_ActorOverlap::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Handle(GetEvent(), OtherActor, SignalOnBeginOverlap);
}

void UBangoTrigger_ActorOverlap::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Handle(GetEvent(), OtherActor, SignalOnEndOverlap);
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


