#pragma once

#include "Bango/Core/BangoTrigger.h"

#include "BangoTrigger_ActorHit.generated.h"

class UBangoInstigatorFilter;

UCLASS(DisplayName="Actor Hit")
class BANGO_API UBangoTrigger_ActorHit : public UBangoTrigger
{
	GENERATED_BODY()

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/** By default the event will use itself as the source of hit triggers. Pick another actor to listen for hit triggers from that actor instead. */
	UPROPERTY(Category="Settings", DisplayName = "Use Target Actor", Category="Settings", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

	UPROPERTY()
	bool bUseTargetActor;
	
	/** Use this to determine which hit events to use and which to ignore. */
	UPROPERTY(Category="Settings", EditAnywhere, Instanced)
	UBangoInstigatorFilter* InstigatorFilter;

	/** How to trigger the event when a hit occurs. */
	UPROPERTY(Category="Settings", DisplayName="On Hit", EditAnywhere)
	EBangoTriggerType TriggerType = EBangoTriggerType::ActivateEvent;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	/***/
	TWeakObjectPtr<AActor> SubscribedActor = nullptr;

public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

public:
	/***/
	void SetTargetActor(AActor* NewTargetActor);

protected:
	UFUNCTION()
	void OnActorHit(AActor* HitActor, AActor* InstigatorActor, FVector NormalImpulse, const FHitResult& Hit);
};