﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "BangoEvent.generated.h"

class UBangoEventProcessor;
class UObject;
class UBangoAction;
class UBangoTriggerCondition;
class UBangoPlungerComponent;
class FCanvasTextItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBangoEventActivated, ABangoEvent*, Event, UObject*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBangoEventDeactivated, ABangoEvent*, Event, UObject*, Instigator);

UENUM()
enum class EBangoEventType : uint8
{
	Bang					UMETA(ToolTip="Runs all action's start functions when triggered."),
	Toggle					UMETA(ToolTip=""),
	Instanced				UMETA(ToolTip=""),
	MAX						UMETA(Hidden)
};

UENUM()
enum class EBangoToggleDeactivateCondition : uint8
{
	AllInstigatorsMustDeactivate,
	AnyInstigatorCanDeactivate,
	OriginalInstigatorDeactivates,
	AnythingCanDeactivate,
	MAX						UMETA(Hidden)
};

UENUM()
enum class EBangoWorldTimeType : uint8
{
	GameTime,
	UnpausedTime,
	RealTime,
	AudioTime,
	MAX
};

#if WITH_EDITOR
enum class EBangoEventState : uint8
{
	NONE		= 0			UMETA(Hidden),
	Initialized = 1 << 0,
	Active		= 1 << 1, 
	Frozen		= 1 << 2,
	Expired		= 1 << 3
};

inline uint8 operator|(EBangoEventState Left, EBangoEventState Right)
{
	return (uint8)Left | (uint8)Right;
}

struct FBangoEventStateFlag
{
	uint8 Value = 0;

	void SetFlag(EBangoEventState Flag, bool NewValue)
	{
		if (NewValue)
		{
			SetFlag(Flag);
		}
		else
		{
			ClearFlag(Flag);
			ClearFlag(Flag);
		}
	}
	
	void SetFlag(EBangoEventState In)
	{
		Value |= (uint8)In;
	}

	void ClearFlag(EBangoEventState In)
	{
		Value &= ~(uint8)In;
	}

	bool HasFlag(EBangoEventState In) const
	{
		return (Value & (uint8)In) == (uint8)In;
	}

	bool HasFlag(uint8 In) const
	{
		return (Value & In);
	}
	
	void ToggleFlag(EBangoEventState In)
	{
		Value ^= (uint8)In;
	}
};
#endif

USTRUCT()
struct FBangoEventInstigatorActions
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<UBangoAction*> Actions;
};

/**
 * 
 */
UCLASS()
class BANGO_API ABangoEvent : public AActor
{
	GENERATED_BODY()
	
	// CONSTRUCTION
	// ============================================================================================
public:
	ABangoEvent();

	// SETTINGS
	// ============================================================================================
#if WITH_EDITORONLY_DATA
protected:
	/**  */
	UPROPERTY(Category="Bango|Display", EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	/**  */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomColor"))
	FLinearColor CustomColor;

	/**  */
	UPROPERTY()
	bool bUseCustomColor = false;
#endif

private:
	/** Bang events are simply triggered over and over. Toggle events turn on and off. Instanced events turn on and off, but spawn and run new instances of their actions for each instigator. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere)
	EBangoEventType Type;

	/** When set, will run actions' stop functions immediately after running their start functions, and when unset will only run actions' start functions. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, meta=(EditCondition="Type==EBangoEventType::Bang", EditConditionHides))
	bool bRunActionStopFunctions = false;

	/**  */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, DisplayName="Deactivate When", meta=(EditCondition="Type==EBangoEventType::Toggle", EditConditionHides))
	EBangoToggleDeactivateCondition DeactivateCondition;
	
	/** When set, this event can only be triggered this many times before it becomes expired. Expired events will ignore any trigger signals. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, meta=(EditCondition="bUseTriggerLimit", ClampMin=1))
	int32 TriggerLimit = 1;

	/**  */
	UPROPERTY()
	bool bUseTriggerLimit = true;

	/** When set, start actions will be delayed by the specified length of time. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, meta=(EditCondition="bUseStartTriggerDelay", ClampMin = 0.0))
	double StartTriggerDelay = 0;

	/**  */
	UPROPERTY()
	bool bUseStartTriggerDelay = false;

	/** When set, stop actions will be delayed by the specified length of time. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, meta=(EditCondition="bUseStopTriggerDelay", ClampMin = 0.0))
	double StopTriggerDelay = 0;

	/**  */
	UPROPERTY()
	bool bUseStopTriggerDelay = false;

	/**  */
	UPROPERTY(Category="Bango|Behavior", EditAnywhere, Instanced)
	TArray<TObjectPtr<UBangoTriggerCondition>> StartTriggers;

	/** Actions to run when event is triggered, or turns on for an on/off event. */
	UPROPERTY(Category="Bango|Behavior", EditAnywhere, Instanced)
	TArray<TObjectPtr<UBangoAction>> Actions;
	
	/**  */
	UPROPERTY(Category="Bango|Behavior", EditAnywhere, Instanced, meta=(EditCondition="Type != EBangoEventType::Bang", EditConditionHides))
	TArray<TObjectPtr<UBangoTriggerCondition>> StopTriggers;

	/**  */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;

	/** When set, freezes the event after it expires and runs out of instigators, typically to disable triggers and save CPU. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere, meta=(EditCondition="bUseTriggerLimit", HideEditConditionToggle, EditConditionHides))
	bool bFreezeWhenExpired = true;
	
	/** How to measure trigger delay times or trigger hold times. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	EBangoWorldTimeType TimeType = EBangoWorldTimeType::GameTime;
	
	// SETTINGS GETTERS AND SETTERS
	// ------------------------------------------
#if WITH_EDITORONLY_DATA
public:
	FText GetDisplayName();
	
	bool GetUsesCustomColor();
	
	FLinearColor GetCustomColor();
#endif
	
public:
	UFUNCTION(BlueprintCallable)
	bool GetStartsFrozen();
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerLimit();

	UFUNCTION(BlueprintCallable)
	void SetTriggerLimit(int32 NewTriggerLimit);
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerCount();
	
	UFUNCTION(BlueprintCallable)
	double GetStartTriggerDelay();
	
	UFUNCTION(BlueprintCallable)
	double GetStopTriggerDelay();

	UFUNCTION(BlueprintCallable)
	bool IsToggleType();

	UFUNCTION(BlueprintCallable)
	bool IsInstancedType();

	bool IsBangType();
	
	UFUNCTION(BlueprintCallable)
	EBangoEventType GetType();

	UFUNCTION(BlueprintCallable)
	EBangoToggleDeactivateCondition GetDeactivateCondition();

	UFUNCTION(BlueprintCallable)
	bool GetRunsActionStops();

	const TArray<TObjectPtr<UBangoAction>>& GetActions();
	// ============================================================================================
	// State
	// ============================================================================================
protected:
	UPROPERTY(Transient)
	TObjectPtr<UBangoEventProcessor> EventProcessor; 
	
	/**  */
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bFrozen = false;
	
	/**  */
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	int32 TriggerCount = 0;
	
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	double LastStartActionsTime = -999;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	double LastStopActionsTime = -999;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventActivated OnBangoEventActivated;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventDeactivated OnBangoEventDeactivated;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bActivateRequestQueued = false;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bDeactivateRequestQueued = false;

	UPROPERTY(Transient)
	FTimerHandle StartTimerHandle;
	
	// ------------------------------------------
	// State Getters
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool GetIsFrozen();

	UFUNCTION(BlueprintCallable)
	bool GetIsExpired();

	UFUNCTION(BlueprintCallable)
	double GetLastStartActionsTime();

	UFUNCTION(BlueprintCallable)
	double GetLastStopActionsTime();
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void ResetTriggerCount(bool bUnfreeze = true);

	UFUNCTION(BlueprintCallable)
	void SetFrozen(bool bFreeze);

public:
	UFUNCTION(BlueprintCallable)
	void ActivateFromTrigger(UObject* NewInstigator);

	void PerformActivateFromTrigger(UObject* NewInstigator);
	
	UFUNCTION(BlueprintCallable)
	void DeactivateFromTrigger(UObject* OldInstigator);

	void PerformDeactivateFromTrigger(UObject* OldInstigator);
	
protected:
	void EnableTriggers(TArray<UBangoTriggerCondition*>& Triggers);

	void DisableTriggers(TArray<UBangoTriggerCondition*>& Triggers);
	
	// ============================================================================================
	// Editor |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// ============================================================================================
#if WITH_EDITOR
public:
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> BangoEventsShowFlag;

private:
	FBangoEventStateFlag CurrentState;

	FDelegateHandle DebugDrawService_Editor;
	
	FDelegateHandle DebugDrawService_Game;
	
	TObjectPtr<UBangoPlungerComponent> PlungerComponent;

public:
	const FBangoEventStateFlag& GetState() const;
	
	bool HasCurrentState(EBangoEventState State);
	
	void OnConstruction(const FTransform& Transform) override;

	void UpdateProxyState();

protected:
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont);

	bool GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation);
	
	FCanvasTextItem GetDebugHeaderText(const FVector& ScreenLocationCentre);

	TArray<FCanvasTextItem> GetDebugDataText(const FVector& ScreenLocationCentre, TDelegate<TArray<FString>()> DataGetter);
	
	TArray<FString> GetDebugDataString_Editor();

	TArray<FString> GetDebugDataString_Game();
#endif
};










// TODO: types of trigger: contact, overlap, kill, global event message, 
// TODO: handle multiple simultaneous overlaps/triggers
// TODO: colorize colliders
// TODO: trigger from different sources - overlaps, impacts of other actors?, messages from other actors?
// TODO: editor icons or some way to distinguish
// TODO: trigger type selector? Have actions provide an icon/identifier?
// TODO: need some way to freeze/unfreeze an event? So that if you want to change the world... one event could unfreeze another event.
