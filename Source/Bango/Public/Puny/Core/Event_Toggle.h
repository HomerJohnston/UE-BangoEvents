﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once
#include "Puny/Core/Event.h"

#include "Event_Toggle.generated.h"

UENUM(BlueprintType)
enum class EPunyEvent_ToggleState : uint8
{
	Null,
	Activated,
	Deactivated,
};

UENUM(BlueprintType)
enum class EPunyEvent_ToggleDeactivateCondition : uint8
{
	AnyDeactivateTrigger,
	AnyInstigatorRemoved,
	AllInstigatorsRemoved,
	OriginalInstigatorRemoved,
	MAX						UMETA(Hidden)
};

UCLASS(DisplayName="Toggle")
class BANGO_API UPunyEvent_Toggle : public UPunyEvent
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UPunyEvent_Toggle();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/** If true, the event will be activated with its owning actor as an instigator on BeginPlay. */
	UPROPERTY(Category="Settings", EditAnywhere)
	bool bStartActivated = false;
	
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	EPunyEvent_ToggleDeactivateCondition DeactivateCondition;

	UPROPERTY(Category="Advanced", EditAnywhere, meta=(EditCondition="false", EditConditionHides))
	uint8 UPunyEvent_Toggle_AdvancedCategorySorter;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================
	UPROPERTY(Category="Debug", VisibleAnywhere)
	EPunyEvent_ToggleState ToggleState;
	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------
public:
	bool GetIsExpired() override;
	
	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	void Init() override;
	
public:
	EPunyEventSignalType RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal) override;

protected:
	bool Activate(UObject* Instigator);

	bool Deactivate(UObject* Instigator);

	bool SetToggleState(EPunyEvent_ToggleState NewState);
	
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
public:
	FLinearColor GetDisplayBaseColor() override;

	void ApplyColorEffects(FLinearColor& Color) override;
	
	bool GetIsPlungerPushed() override;

	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================

#if WITH_EDITOR
	void AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data) override;
#endif
};