#pragma once

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Puny/PunyTriggerSignal.h"

#include "PunyTrigger.generated.h"

class UPunyEvent;
class UPunyEventComponent;
struct FBangoDebugTextEntry;

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPunyTriggerSignalDelegate, UPunyTrigger*, Trigger, FPunyTriggerSignal, Signal);

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UPunyTrigger : public UObject
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
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
private:
	FPunyTriggerSignalDelegate TriggerSignal;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	/**  */
	void SetEnabled(bool bEnabled);

	/**  */
	void RegisterEvent(UPunyEvent* Event);

	/**  */
	void UnregisterEvent(UPunyEvent* Event);
	
protected:
	/** Set up logic to enable the trigger here (subscribe to world events, start running timers or ticking logic, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Enable();

	/** Set up logic to disable the trigger here (unsubscribe to world events, stop running timers or ticking logic, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Disable();

	void SendSignal(FPunyTriggerSignal Signal);

protected:
	UPunyEventComponent* GetEventComponent();

	AActor* GetActor();

	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================

	#if WITH_EDITORONLY_DATA
private:
	/** Set to override the editor display name. */
	UPROPERTY(Category="Advanced", DisplayName="Display Name Override", EditAnywhere, meta=(EditCondition="bUseDisplayName", DisplayPriority=-2))
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

#endif
};
