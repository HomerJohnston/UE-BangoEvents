// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Puny/Trigger.h"

#include "Trigger_ActorOverlap.generated.h"

UCLASS(DisplayName="Actor Overlap")
class BANGO_API UPunyTrigger_ActorOverlap : public UPunyTrigger
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

private:
	/** Signal to event upon beginning overlap. */
	UPROPERTY(Category="Settings", EditAnywhere)
	EPunyTriggerSignalType OnBeginOverlap;

	/** Signal to event upon ending overlap. */
	UPROPERTY(Category="Settings", EditAnywhere)
	EPunyTriggerSignalType OnEndOverlap;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetActor = false;
	
	/** By default the event will use itself as the source of overlap triggers. Pick another actor to listen for overlap triggers from that actor instead. */
	UPROPERTY(DisplayName = "Use Overlap Events From Actor", Category="Settings", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetComponent = false;

	/** Optionally choose a specific component to listen for overlap triggers from. Note: only works on self actor, due to a bug in FComponentReference. */
	UPROPERTY(DisplayName = "Use Overlap Events From Component", Category="Settings", EditAnywhere, meta=(EditCondition = "bUseTargetComponent", UseComponentPicker))
	FComponentReference TargetComponent;

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================

private:
	TWeakObjectPtr<UPrimitiveComponent> SubscribedComponent = nullptr;
	
	TWeakObjectPtr<AActor> SubscribedActor = nullptr;

	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------

	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------

	// ============================================================================================
	// METHODS
	// ============================================================================================

public:
	virtual void Disable_Implementation() override;
	
	virtual void Enable_Implementation() override;

protected:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnActorBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor);

	UFUNCTION()
	void OnActorEndOverlap(AActor* OverlapActor, AActor* InstigatorActor);
	
	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
#if WITH_EDITOR
public:
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	bool HasValidSetup() override;
#endif
};