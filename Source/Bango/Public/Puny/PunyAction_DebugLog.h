﻿#pragma once
#include "PunyAction.h"

#include "PunyAction_DebugLog.generated.h"

struct FPunyEventSignal;
enum class EPunyEventSignalType : uint8;
UCLASS(DisplayName="Debug Log")
class BANGO_API UPunyAction_DebugLog : public UPunyAction
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UPunyAction_DebugLog();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseActivateMessage = false;

	UPROPERTY(Category="Settings", DisplayName="Override Activate Message", EditAnywhere, meta=(EditCondition="bUseActivateMessage"))
	FString ActivateMessage;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseDeactivateMessage;
	
	UPROPERTY(Category="Settings", DisplayName="Override Deactivate Message", EditAnywhere, meta=(EditCondition="bUseDeactivateMessage"))
	FString DeactivateMessage;

	// TODO: Print to screen options, or enum for destinations
	
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
	void HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal) override;
	
protected:
	FText GetEventName();

	FString GetDefaultMessage(FPunyEventSignal Signal);
	
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
