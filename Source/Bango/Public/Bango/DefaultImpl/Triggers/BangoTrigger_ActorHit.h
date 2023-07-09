#pragma once

#include "Bango/Trigger/BangoBangTrigger.h"

#include "BangoTrigger_ActorHit.generated.h"

class UBangoInstigatorFilter;

enum class EBangoSignal : uint8;

UCLASS(DisplayName="Actor Hit")
class BANGO_API UBangoTrigger_ActorHit : public UBangoBangTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_ActorHit();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/** By default the event will use itself as the source of hit triggers. Pick another actor to listen for hit triggers from that actor instead. */
	UPROPERTY(Category="Settings", DisplayName = "Hit Source Actor", Category="Settings", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

	UPROPERTY()
	bool bUseTargetActor;
	
	/** Use this to determine which hit events to use and which to ignore. */
	UPROPERTY(Category="Settings", EditAnywhere, Instanced)
	UBangoInstigatorFilter* InstigatorFilter;

	/** How to trigger the event when a hit occurs. */
	UPROPERTY(Category="Settings", DisplayName="On Hit", EditAnywhere)
	EBangoSignal TriggerAction;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	/***/
	TWeakObjectPtr<AActor> SubscribedActor = nullptr;

	// ============================================================================================
	// API
	// ============================================================================================
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