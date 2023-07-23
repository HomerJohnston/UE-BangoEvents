#include "Puny/Default/Trigger_ActorOverlap.h"

#include "Bango/Utility/Log.h"

void UPunyTrigger_ActorOverlap::Enable_Implementation()
{
	if (bUseSpecificComponent && bUseTargetActor)
	{
		UE_LOG(Bango, Error, TEXT("UPunyTrigger_ActorOverlap is set to use both a component and a target actor, can only be one or the other!"));
		return;
	}

	UActorComponent* SpecificComponent = bUseSpecificComponent ? Component.GetComponent(GetActor()) : nullptr;
	
	if (bUseSpecificComponent && !IsValid(SpecificComponent))
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
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(SpecificComponent);

		if (IsValid(PrimitiveComponent))
		{
			PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
			PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

			SubscribedComponent = PrimitiveComponent;
		}

		return;
	}
	
	AActor* ActorToUse = (bUseTargetActor && IsValid(TargetActor)) ? TargetActor : GetActor();
	
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
	
	ActorToUse->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnActorBeginOverlap);
	ActorToUse->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnActorEndOverlap);

	SubscribedActor = ActorToUse;
}

void UPunyTrigger_ActorOverlap::Disable_Implementation()
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
	
	SubscribedActor->OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnActorBeginOverlap);
	SubscribedActor->OnActorEndOverlap.RemoveDynamic(this, &ThisClass::OnActorEndOverlap);

	SubscribedActor = nullptr;
}

void UPunyTrigger_ActorOverlap::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SendSignal(FPunyTriggerSignal(OnBeginOverlap, OtherComp));
}

void UPunyTrigger_ActorOverlap::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SendSignal(FPunyTriggerSignal(OnEndOverlap, OtherComp));
}


void UPunyTrigger_ActorOverlap::OnActorBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	SendSignal(FPunyTriggerSignal(OnBeginOverlap, InstigatorActor));
}

void UPunyTrigger_ActorOverlap::OnActorEndOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	SendSignal(FPunyTriggerSignal(OnEndOverlap, InstigatorActor));
}
