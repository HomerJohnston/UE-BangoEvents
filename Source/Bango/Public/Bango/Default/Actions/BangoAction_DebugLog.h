// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/BangoAction.h"

#include "BangoAction_DebugLog.generated.h"

struct FBangoEventSignal;
enum class EBangoEventSignalType : uint8;

UENUM()
enum class EBangoAction_DebugLog_PrintTo
{
	Log,
	LogAndScreen,
	Screen,
};

UCLASS(DisplayName="Debug Log")
class BANGO_API UBangoAction_DebugLog : public UBangoAction
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
public:
	UBangoAction_DebugLog();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

protected:
	/**  */
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bOverrideActivateMessage = false;

	/**  */
	UPROPERTY(DisplayName="Override Activate Message", EditAnywhere, meta=(EditCondition="bOverrideActivateMessage"))
	FString ActivateMessage;

	/**  */
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bOverrideDeactivateMessage;
	
	/**  */
	UPROPERTY(DisplayName="Override Deactivate Message", EditAnywhere, meta=(EditCondition="bOverrideDeactivateMessage"))
	FString DeactivateMessage;

	/**  */
	UPROPERTY(EditAnywhere, Category="Advanced")
	EBangoAction_DebugLog_PrintTo PrintTo = EBangoAction_DebugLog_PrintTo::Log;

	/**  */	
	UPROPERTY(EditAnywhere, meta=(EditCondition="PrintTo != EBangoAction_DebugLog_PrintTo::Log", EditConditionHides))
	int32 OnScreenKey = -1;
	
	/**  */
	UPROPERTY(EditAnywhere, meta=(EditCondition="PrintTo != EBangoAction_DebugLog_PrintTo::Log", EditConditionHides))
	float OnScreenDisplayTime = 1.0f;

	/**  */
	UPROPERTY(EditAnywhere, meta=(EditCondition="PrintTo != EBangoAction_DebugLog_PrintTo::Log", EditConditionHides))
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
	UFUNCTION()
	void PrintActivationMessage(UBangoEvent* Event, UObject* Instigator);

	UFUNCTION()
	void PrintDeactivationMessage(UBangoEvent* Event, UObject* Instigator);

	void Handle(bool& bUseCustomMessage, FString& CustomMessage, EBangoEventSignalType SignalType, UObject* Instigator);

	void Print(FString& Message);
	
protected:
	FText GetEventName();

	FString GetDefaultMessage(EBangoEventSignalType SignalType, UObject* Instigator);
	
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
