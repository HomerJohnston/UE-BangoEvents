// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "BangoEvent.generated.h"

class UBangoEventProcessor;
class UObject;
class UBangoAction;
class UBangoTrigger;
class UBangoPlungerComponent;
class FCanvasTextItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBangoEventActivated, ABangoEvent*, Event, UObject*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBangoEventDeactivated, ABangoEvent*, Event, UObject*, Instigator);

UENUM()
enum class EBangoEventType : uint8
{
	Bang					UMETA(ToolTip="Bang Events have no concept of state: Triggers can freely execute Start and Stop of the Event's Actions whenever they fire Activate or Deactivate."),
	Toggle					UMETA(ToolTip="Toggle Events are either activated or deactivated, and can only execute Start and Stop of the Event's Actions when the state changes. If the Actions include a delay, deactivating the event before the Action starts will cause that action to abort and not run."),
	//Instanced				UMETA(ToolTip="Instanced Tooltip"),
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

	TMap<int, FBangoEventInstigatorActions*> Test;
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
	UPROPERTY(Category="Bango|Display", DisplayName="Event Display Name", EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	/**  */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomColor"))
	FLinearColor CustomColor = FColor::White;

	/**  */
	UPROPERTY()
	bool bUseCustomColor = false;
#endif

private:
	/** Bang events are simply triggered but can be interrupted if they have a delay. Toggle events turn on and off. Instanced events turn on and off, but spawn and run new instances of their actions for each instigator. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere)
	EBangoEventType Type;

	/**  */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, DisplayName="Deactivate When", meta=(EditCondition="Type==EBangoEventType::Toggle", EditConditionHides))
	EBangoToggleDeactivateCondition DeactivateCondition;
	
	/** When set, this event can only be triggered this many times before it becomes expired. Expired events will ignore any trigger signals. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, meta=(EditCondition="bUseActivationLimit", ClampMin=1))
	int32 ActivationLimit = 1;

	/**  */
	UPROPERTY()
	bool bUseActivationLimit = true;

	/** Runs the Start function of all Actions */
	UPROPERTY(Category="Bango|Behavior", EditAnywhere, Instanced)
	TArray<TObjectPtr<UBangoTrigger>> Triggers;

	/** Actions to run when event is triggered, or turns on for an on/off event. */
	UPROPERTY(Category="Bango|Behavior", EditAnywhere, Instanced)
	TArray<TObjectPtr<UBangoAction>> Actions;
	
	/**  */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;

	/** When set, freezes the event after it expires and runs out of instigators, typically to disable triggers and save CPU. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere, meta=(EditCondition="bUseActivationLimit", HideEditConditionToggle, EditConditionHides))
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
	bool IsToggleType();

	UFUNCTION(BlueprintCallable)
	bool IsInstancedType();

	bool IsBangType();
	
	UFUNCTION(BlueprintCallable)
	EBangoEventType GetType();

	UFUNCTION(BlueprintCallable)
	EBangoToggleDeactivateCondition GetDeactivateCondition();

	UFUNCTION(BlueprintCallable)
	const TArray<UBangoAction*>& GetActions();
	
	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	UPROPERTY(Transient)
	TObjectPtr<UBangoEventProcessor> EventProcessor; 
	
	/**  */
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bFrozen = false;
	
	/**  */
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	int32 ActivationCount = 0;
	
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	double LastActivationTime = -999;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	double LastDeactivationTime = -999;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventActivated OnBangoEventActivated;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventDeactivated OnBangoEventDeactivated;

	// ------------------------------------------
	// STATE GETTERS
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool GetIsFrozen();

	UFUNCTION(BlueprintCallable)
	bool GetIsExpired();

	UFUNCTION(BlueprintCallable)
	double GetLastActivationTime();

	UFUNCTION(BlueprintCallable)
	double GetLastDeactivationTime();

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
	void Activate(UObject* NewInstigator);

	UFUNCTION(BlueprintCallable)
	void Deactivate(UObject* OldInstigator);

protected:
	void EnableTriggers();

	void DisableTriggers();
	
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
	
	double GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation);
	
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
