// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Core/BangoTriggerSignal.h"

#include "BangoTrigger_OLD.generated.h"

class UBangoEvent;
class UBangoEventComponent;
class UBangoInstigatorFilter;
struct FBangoDebugTextEntry;

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_TwoParams(FBangoTriggerSignalDelegate, UBangoTrigger_OLD*, Trigger, FBangoTriggerSignal, Signal);

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoTrigger_OLD : public UObject
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/** Filter for overlap events. */
	UPROPERTY(Category = "Default", EditAnywhere, Instanced)
	UBangoInstigatorFilter* InstigatorFilter;
	
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
private:
	FBangoTriggerSignalDelegate TriggerSignal;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	/**  */
	void SetEnabled(bool bEnabled);

	/**  */
	void RegisterEvent(UBangoEvent* Event);

	/**  */
	void UnregisterEvent(UBangoEvent* Event);
	
protected:
	/** Set up logic to enable the trigger here (subscribe to world events, start running timers or ticking logic, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Enable();

	/** Set up logic to disable the trigger here (unsubscribe to world events, stop running timers or ticking logic, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Disable();

	void SendSignal(FBangoTriggerSignal Signal);

protected:
	UBangoEventComponent* GetEventComponent();

	AActor* GetActor();

	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================

	#if WITH_EDITORONLY_DATA
private:
	/** Set to override the editor display name. */
	UPROPERTY(Category="Advanced", DisplayName="Display Name Override", EditAnywhere, meta=(EditCondition="bUseDisplayName", DisplayPriority=-1))
	FText DisplayName;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseDisplayName = false;
#endif
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
	UFUNCTION(BlueprintNativeEvent)
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont);

	UFUNCTION(BlueprintCallable)
	FText GetDisplayName() const;

	virtual void AppendDebugData(TArray<FBangoDebugTextEntry>& Data);

	virtual bool HasValidSetup();
#endif
};
