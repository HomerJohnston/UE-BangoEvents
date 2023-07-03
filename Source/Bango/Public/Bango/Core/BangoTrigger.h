﻿#pragma once

#include "UObject/Object.h"

#include "BangoTrigger.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FTriggerDelegate, UObject*, NewInstigator);

UENUM(BlueprintType)
enum class EBangoTriggerBehavior : uint8
{
	ActivatesAndDeactivates,
	ActivatesOnly,
	DeactivatesOnly,
};

class ABangoEvent;
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoTrigger : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	UPROPERTY(Category = "Trigger Settings", EditAnywhere)
	EBangoTriggerBehavior Behavior = EBangoTriggerBehavior::ActivatesAndDeactivates;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	
private:
	/** The owning ABangoEvent will listen for this delegate to fire. */
	UPROPERTY()
	FTriggerDelegate OnTriggerActivation;

	/** The owning ABangoEvent will listen for this delegate to fire. */
	UPROPERTY()
	FTriggerDelegate OnTriggerDeactivation;
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABangoEvent* GetEvent();

public:
	UFUNCTION(BlueprintCallable)
	void SetEnabled(bool bEnabled);

protected:
	/** Perform setup to make the trigger work properly here (e.g. subscribe to events in your game, or turn on a timer to check things, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Enable();
	
	/** Run code to make the trigger stop working here (unsubscribe to events, stop timers, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Disable();

protected:
	/** Run this function to activate this Trigger's Event. */
	UFUNCTION(BlueprintCallable)
	void ActivateEvent(UObject* NewInstigator);

	/** Run this function to deactivate this Trigger's Event. */
	UFUNCTION(BlueprintCallable)
	void DeactivateEvent(UObject* OldInstigator);

#if WITH_EDITOR
public:
	virtual FText GetDisplayName();

	EBangoTriggerBehavior GetBehavior();
#endif
};