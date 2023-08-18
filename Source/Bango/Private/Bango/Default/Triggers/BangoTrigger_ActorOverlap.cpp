#include "Bango/Default/Triggers/BangoTrigger_ActorOverlap.h"

#include "Bango/Utility/BangoLog.h"

void UBangoTrigger_ActorOverlap::Enable_Implementation()
{
	if (bUseTargetComponent && bUseTargetActor)
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use both a component and a target actor, can only be one or the other!"));
		return;
	}

	UActorComponent* SpecificComponent = bUseTargetComponent ? TargetComponent.GetComponent(GetActor()) : nullptr;
	
	if (bUseTargetComponent && !IsValid(SpecificComponent))
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use a component but no component was set!"));
		return;
	}
	
	if (bUseTargetActor && !IsValid(TargetActor))
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ActorOverlap is set to use target actor but no target actor was set!"));
		return;
	}

	if (bUseTargetComponent)
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

void UBangoTrigger_ActorOverlap::Disable_Implementation()
{
	if (bUseTargetComponent)
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

void UBangoTrigger_ActorOverlap::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SendSignal(FBangoTriggerSignal(OnBeginOverlap, OtherComp));
}

void UBangoTrigger_ActorOverlap::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SendSignal(FBangoTriggerSignal(OnEndOverlap, OtherComp));
}

void UBangoTrigger_ActorOverlap::OnActorBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	SendSignal(FBangoTriggerSignal(OnBeginOverlap, InstigatorActor));
}

void UBangoTrigger_ActorOverlap::OnActorEndOverlap(AActor* OverlapActor, AActor* InstigatorActor)
{
	SendSignal(FBangoTriggerSignal(OnEndOverlap, InstigatorActor));
}

void UBangoTrigger_ActorOverlap::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	TArray<FName> TargetActorProperties {"bUseTargetActor", "TargetActor"};
	TArray<FName> TargetComponentProperties {"bUseTargetComponent", "TargetComponent"};
	
	// Ensure that only one setting is in use
	if (bUseTargetActor && bUseTargetComponent)
	{
		if (TargetActorProperties.Contains(PropertyChangedEvent.Property->GetFName()))
		{
			bUseTargetComponent = false;
		}
		else if (TargetComponentProperties.Contains(PropertyChangedEvent.Property->GetFName()))
		{
			bUseTargetActor = false;
		}
		else
		{
			bUseTargetActor = false;
			bUseTargetComponent = false;
		}
	}
}

#if WITH_EDITOR
bool UBangoTrigger_ActorOverlap::HasValidSetup()
{
	return OnBeginOverlap != EBangoTriggerSignalType::None || OnEndOverlap != EBangoTriggerSignalType::None;
}
#endif