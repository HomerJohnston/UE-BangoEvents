﻿#pragma once
#include "PunyEvent.h"

#include "PunyEvent_Bang.generated.h"

UCLASS(DisplayName="Bang")
class BANGO_API UPunyEvent_Bang : public UPunyEvent
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	bool bRespondToDeactivateTriggers = false;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================

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
	EPunyEventSignalType RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal) override;
	
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
};