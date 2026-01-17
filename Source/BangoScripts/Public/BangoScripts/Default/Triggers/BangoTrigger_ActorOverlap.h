// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoScripts/BangoTrigger_OLD.h"
#include "BangoScripts/Components/BangoActorIDComponent.h"

#include "BangoTrigger_ActorOverlap.generated.h"

UCLASS(DisplayName="Actor Overlap")
class BANGOSCRIPTS_API UBangoTrigger_ActorOverlap : public UBangoTrigger_OLD
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
	UPROPERTY(EditAnywhere)
	EBangoTriggerSignalType OnBeginOverlap;

	/** Signal to event upon ending overlap. */
	UPROPERTY(EditAnywhere)
	EBangoTriggerSignalType OnEndOverlap;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetComponent = false;

	/** By default the event will listen for overlap events from all overlapping components on its actor. Use this to only listen for overlap events from a specific component. */ // TODO Note: only works on self actor, due to a bug in FComponentReference.
	UPROPERTY(DisplayName = "Use Overlaps From Component", EditAnywhere, meta=(EditCondition = "bUseTargetComponent", UseComponentPicker))
	FComponentReference TargetComponent;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetActor = false;
	
	/** By default the event will use itself as the source of overlap triggers. Use this to listen for overlap events from another actor instead. */
	UPROPERTY(DisplayName = "Use Other Actor", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

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