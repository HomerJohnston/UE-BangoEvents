// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Bango/Core/BangoSignal.h"

#include "BangoTrigger.generated.h"

class ABangoEvent;
struct  FBangoDebugTextEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTriggerDelegate, EBangoSignal, Signal, UObject*, NewInstigator);

/** Basic trigger class, usable on any event type. */
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoTrigger : public UObject
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/** Whether to use timed signal features or not. */
	UPROPERTY(EditAnywhere, Category="Advanced")
	bool bUseSignalDelays = false;
	
	/** Set delays to use before sending trigger signals. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(ReadOnlyKeys, UIMin = 0.0, UIMax = 60.0, EditCondition="bUseSignalDelays", EditConditionHides))
	TMap<EBangoSignal, float> SignalDelays;

	/** If true, broadcasting a Deactivate signal while an Activate signal is pending will cancel the Activate signal, and vice versa. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides))
	bool bCancelOpposingSignals = true;

	/** If true, when other triggers send activate/deactivate signals to the event, this trigger will react. This enables other triggers to cancel this trigger's pending delayed signals. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides))
	bool bReactToEventSignalling = true;

	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides))
	bool bAllowRestartingTimer = false;
	// ============================================================================================
	// STATE
	// ============================================================================================
	
public:
	// TODO can I private this and expose binding functions?
	/** Call this delegate to control the event. */
	UPROPERTY()
	FTriggerDelegate TriggerSignal;

	UPROPERTY()
	TMap<EBangoSignal, FTimerHandle> DelayedSignalTimers; 

	UPROPERTY()
	bool bIgnoreEventSignalling = false;
	// ------------------------------------------
	// State Getters/Setters
	// ------------------------------------------
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Get Event")
	ABangoEvent* GetEventBP();

	ABangoEvent* GetEvent();
	
	template<class T>
	T* GetEvent();

	// ============================================================================================
	// API
	// ============================================================================================
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
	UFUNCTION(BlueprintCallable)
	void SendTriggerSignal(EBangoSignal Signal, UObject* NewInstigator);

	bool PerformDelayedSignal(EBangoSignal Signal, UObject* NewInstigator);

	bool CancelOpposingSignal(EBangoSignal Signal);
	
	void SendTriggerSignal_Delayed(EBangoSignal Signal, TWeakObjectPtr<UObject> NewInstigator);

	UFUNCTION()
	void ReactToEventSignal(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator);
	
	// ============================================================================================
	// EDITOR
	// ============================================================================================
#if WITH_EDITOR
public:
	UFUNCTION(BlueprintNativeEvent)
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont);

	void DebugPrintTimeRemaining(TArray<FBangoDebugTextEntry>& Data, const FString& Label, EBangoSignal Signal);

	virtual void AppendDebugData(TArray<FBangoDebugTextEntry>& Data);
	
public:
	virtual FText GetDisplayName();
#endif
};

template <class T>
T* UBangoTrigger::GetEvent()
{
	return Cast<T>(GetOuter());
}
