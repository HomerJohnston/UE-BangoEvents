// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "BangoEvent.generated.h"

class UObject;
class UBangoAction;
class UBangoTriggerCondition;
class UBangoPlungerComponent;
class FCanvasTextItem;

UENUM()
enum class EBangoEventType : uint8
{
	Bang,
	Toggle,
	Instanced,
	MAX
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

enum class EBangoEventState : uint8
{
	NONE		= 0 UMETA(Hidden),
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

USTRUCT()
struct FBangoEventInstigatorActions
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<UBangoAction*> Actions;
};

// TODO multiplayer compatibility - run on server, on client, on both
/**
 * 
 */
UCLASS()
class BANGO_API ABangoEvent : public AActor
{
	GENERATED_BODY()
	
	// ============================================================================================
	// Constructor/Destructor
	// ============================================================================================
public:
	ABangoEvent();

	// ============================================================================================
	// Settings
	// ============================================================================================
private:
#if WITH_EDITORONLY_DATA
	/**  */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	FText DisplayName;
#endif
	UPROPERTY(Category="Bango", EditAnywhere)
	EBangoEventType Type;
	
	/** When set, this event can only be triggered this many times before it becomes expired. Expired events will ignore any trigger signals. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bUseTriggerLimit", ClampMin=1))
	int32 TriggerLimit = 1;

	UPROPERTY()
	bool bUseTriggerLimit = true;

	/** When set, start actions will be delayed by the specified length of time. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bUseStartTriggerDelay", ClampMin = 0.0))
	double StartTriggerDelay = 0;

	UPROPERTY()
	bool bUseStartTriggerDelay = false;

	/** When set, stop actions will be delayed by the specified length of time. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bUseStopTriggerDelay", ClampMin = 0.0))
	double StopTriggerDelay = 0;

	UPROPERTY(meta=(EditCondition="bUseStopTriggerDelay", EditConditionHides))
	bool bUseStopTriggerDelay = false;

	/**  */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoTriggerCondition*> StartTriggers;

	/** Actions to run when event is triggered, or turns on for an on/off event. */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoAction*> Actions;
	
	/**  */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced, meta=(EditCondition="Type != EBangoEventType::Bang", EditConditionHides))
	TArray<UBangoTriggerCondition*> StopTriggers;

	/**  */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;

	/** When set, freezes the event after it expires and runs out of instigators, typically to disable triggers and save CPU. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere, meta=(EditCondition="bUseTriggerLimit", HideEditConditionToggle, EditConditionHides))
	bool bFreezeWhenExpired = true;
	
	/** How to measure trigger delay times or trigger hold times. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	EBangoWorldTimeType TimeType = EBangoWorldTimeType::GameTime;
	
	// ------------------------------------------
	// Settings Getters
	// ------------------------------------------
public:
#if WITH_EDITORONLY_DATA
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetDisplayName();
#endif
	
	UFUNCTION(BlueprintCallable)
	bool GetStartsFrozen();
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerLimit();

	UFUNCTION(BlueprintCallable)
	int32 GetTriggerCount();
	
	UFUNCTION(BlueprintCallable)
	double GetStartTriggerDelay();
	
	UFUNCTION(BlueprintCallable)
	double GetStopTriggerDelay();

	UFUNCTION(BlueprintCallable)
	bool GetToggles();

	UFUNCTION(BlueprintCallable)
	bool GetIsInstanced();

	UFUNCTION(BlueprintCallable)
	EBangoEventType GetType();
	
	// ============================================================================================
	// State
	// ============================================================================================
protected:
	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	bool bFrozen = false;
	
	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	int32 TriggerCount = 0;
	
	/** Instigators which are actively triggering an on/off event. */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	TArray<UObject*> Instigators;

	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	double LastStartActionsTime = -999;

	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	double LastStopActionsTime = -999;
	
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	TMap<UObject*, FBangoEventInstigatorActions> InstancedActions;
	
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

	void ResetTriggerCount(bool bUnfreeze = true);

	UFUNCTION(BlueprintCallable)
	void SetFrozen(bool bNewFrozen);

protected:
	UFUNCTION()
	void ActivateFromTrigger(UObject* NewInstigator);

	UFUNCTION()
	void DeactivateFromTrigger(UObject* OldInstigator);

	void EnableTriggers(TArray<UBangoTriggerCondition*>& Triggers);

	void DisableTriggers(TArray<UBangoTriggerCondition*>& Triggers);
	
	void StartActions(UObject* NewInstigator);
	
	void StopActions(UObject* OldInstigator);

	// ============================================================================================
	// Editor |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// ============================================================================================
#if WITH_EDITORONLY_DATA
public:
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> BangoEventsShowFlag;

private:
	FBangoEventStateFlag CurrentState;

	FDelegateHandle DebugDrawService_Editor;
	
	FDelegateHandle DebugDrawService_Game;
	
	UPROPERTY(Transient)
	TObjectPtr<UBangoPlungerComponent> PlungerComponent;
#endif

#if WITH_EDITOR
public:
	const FBangoEventStateFlag& GetState() const;
	
	bool HasCurrentState(EBangoEventState State);
	
	void OnConstruction(const FTransform& Transform) override;

	void UpdateProxyState();

	bool CanEditChange(const FProperty* Property) const override;
	
protected:
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

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
