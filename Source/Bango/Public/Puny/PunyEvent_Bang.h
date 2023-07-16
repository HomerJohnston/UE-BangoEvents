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

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================

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
	void RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal) override;
	
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
	
};