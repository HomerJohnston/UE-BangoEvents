// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Trigger/BangoTrigger_Toggle.h"

#include "BangoTrigger_ActorOverlap.generated.h"

class UBangoInstigatorFilter;

enum class EBangoSignal : uint8;

UCLASS(DisplayName="Actor Overlap")
class BANGO_API UBangoTrigger_ActorOverlap : public UBangoTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_ActorOverlap();
	
protected:
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
	
	UPROPERTY()
	bool bUseTargetActor;
	
	/** By default the event will use itself as the source of overlap triggers. Pick another actor to listen for overlap triggers from that actor instead. */
	UPROPERTY(DisplayName = "Get Overlap Events From Other Actor", Category="Overlap Settings", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

	UPROPERTY()
	bool bUseSpecificComponent;

	UPROPERTY(DisplayName = "Get Overlap Events From Specific Component", Category="Overlap Settings", EditAnywhere, meta=(EditCondition = "bUseSpecificComponent", UseComponentPicker))
	FComponentReference Component;
	
	/** Use this to determine which overlap events to use and which to ignore. */
	UPROPERTY(Category="Overlap Settings", EditAnywhere, Instanced)
	UBangoInstigatorFilter* InstigatorFilter;

	/** Signal to send to event when an overlap begins. */
	UPROPERTY(Category="Overlap Settings", EditAnywhere)
	EBangoSignal SignalOnBeginOverlap;

	/** Signal to send to event when an overlap ends. */
	UPROPERTY(Category="Overlap Settings", EditAnywhere)
	EBangoSignal SignalOnEndOverlap;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	/** */
	TWeakObjectPtr<UPrimitiveComponent> SubscribedComponent = nullptr;
	
	/** */
	TWeakObjectPtr<AActor> SubscribedActor = nullptr;
	
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

public:
	/***/
	void SetTargetActor(AActor* NewTargetActor);
	
protected:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnBeginOverlap(AActor* BangoEventActor, AActor* InstigatorActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlapActor, AActor* InstigatorActor);

	void Handle(AActor* OverlapActor, AActor* InstigatorActor, EBangoSignal Signal);
};
