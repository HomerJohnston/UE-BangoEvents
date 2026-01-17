// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoScripts/BangoTrigger_OLD.h"

#include "BangoTrigger_ActorHit.generated.h"

class UBangoInstigatorFilter;

enum class EBangoTriggerSignal : uint8;

UCLASS(DisplayName="Actor Hit")
class BANGOSCRIPTS_API UBangoTrigger_ActorHit : public UBangoTrigger_OLD
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
	UPROPERTY(DisplayName = "Hit Source Actor", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

	UPROPERTY()
	bool bUseTargetActor;
	
	/** How to trigger the event when a hit occurs. */
	UPROPERTY(DisplayName="On Hit", EditAnywhere)
	EBangoTriggerSignalType SignalOnHit;
	
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
	/** Sets the target actor. */
	void SetTargetActor(AActor* NewTargetActor);

protected:
	UFUNCTION()
	void OnActorHit(AActor* HitActor, AActor* InstigatorActor, FVector NormalImpulse, const FHitResult& Hit);
};