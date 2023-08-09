// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Puny/Action.h"

#include "Action_DebugLog.generated.h"

struct FPunyEventSignal;
enum class EPunyEventSignalType : uint8;

UENUM()
enum class EPunyAction_DebugLog_PrintTo
{
	Log,
	LogAndScreen,
	Screen,
};

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
	/**  */
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseActivateMessage = false;

	/**  */
	UPROPERTY(Category="Settings", DisplayName="Override Activate Message", EditAnywhere, meta=(EditCondition="bUseActivateMessage"))
	FString ActivateMessage;

	/**  */
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseDeactivateMessage;
	
	/**  */
	UPROPERTY(Category="Settings", DisplayName="Override Deactivate Message", EditAnywhere, meta=(EditCondition="bUseDeactivateMessage"))
	FString DeactivateMessage;

	/**  */
	UPROPERTY(EditAnywhere, Category="Settings")
	EPunyAction_DebugLog_PrintTo PrintTo = EPunyAction_DebugLog_PrintTo::Log;

	/**  */	
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="PrintTo != EPunyAction_DebugLog_PrintTo::Log", EditConditionHides))
	int32 OnScreenKey = -1;
	
	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="PrintTo != EPunyAction_DebugLog_PrintTo::Log", EditConditionHides))
	float OnScreenDisplayTime = 1.0f;

	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="PrintTo != EPunyAction_DebugLog_PrintTo::Log", EditConditionHides))
	FColor OnScreenColor = FColor::White;

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
