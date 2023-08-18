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
	EBangoAction_DebugLog_PrintTo PrintTo = EBangoAction_DebugLog_PrintTo::Log;

	/**  */	
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="PrintTo != EBangoAction_DebugLog_PrintTo::Log", EditConditionHides))
	int32 OnScreenKey = -1;
	
	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="PrintTo != EBangoAction_DebugLog_PrintTo::Log", EditConditionHides))
	float OnScreenDisplayTime = 1.0f;

	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="PrintTo != EBangoAction_DebugLog_PrintTo::Log", EditConditionHides))
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
	void Start_Implementation(UBangoEvent* Event, UObject* Instigator) override;

	void Stop_Implementation(UBangoEvent* Event, UObject* Instigator) override;

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
