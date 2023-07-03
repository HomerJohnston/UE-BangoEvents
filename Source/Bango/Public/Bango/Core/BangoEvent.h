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
	Bang					UMETA(ToolTip="Bangs will repeatedly start actions when any trigger activates, and repeatedly stop when any trigger deactivates."),
	Toggle					UMETA(ToolTip="Toggles maintain an activated state, and will only start or stop when the state changes. If any actions have a delay set, deactivating the event before the action starts will cause that action to abort starting."),
	//Instanced				UMETA(ToolTip="Instanced events work like toggle events, but they can be independently activated for multiple instigators."),
	MAX						UMETA(Hidden)
};

UENUM()
enum class EBangoToggleDeactivateCondition : uint8
{
	AllInstigatorsRemoved,
	AnyInstigatorsRemoved,
	OriginalInstigatorRemoved,
	MAX						UMETA(Hidden)
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
struct BANGO_API FBangoEventInstigatorActions
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<UBangoAction*> Actions;

	TMap<int, FBangoEventInstigatorActions*> Test;
};

struct FBangoDebugTextEntry
{
	FString TextL;
	FString TextR;
	FColor Color;

	FBangoDebugTextEntry(FString InTextL, FString InTextR, FColor InColor = FColor::White);
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
	/** Set to override the editor display name. */
	UPROPERTY(Category="Bango|Display", DisplayName="Event Display Name", EditInstanceOnly, BlueprintReadOnly)
	FText DisplayName;
	
	/** Set to override the editor display color. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomColor"))
	FLinearColor CustomColor = FColor::White;

	UPROPERTY()
	bool bUseCustomColor = false;

	/** Set to add a custom mesh display above the event proxy. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomMesh"))
	UStaticMesh* CustomMesh = nullptr;

	UPROPERTY()
	bool bUseCustomMesh = false;

	/** Set to change the display scale of the mesh. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomMesh", EditConditionHides, HideEditConditionToggle, UIMin = 0.1, UIMax = 10.0))
	float CustomMeshScale = 1.0;

	/** Set to change the elevation of the mesh. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomMesh", EditConditionHides, HideEditConditionToggle, UIMin = -1000, UIMax = 1000))
	float CustomMeshOffset = 0.0;

	const float CustomMeshOffsetBase = +100.0;
#endif

private:
	/** Bang events are simply triggered but can be interrupted if they have a delay. Toggle events turn on and off. Instanced events turn on and off, but spawn and run new instances of their actions for each instigator. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere)
	EBangoEventType Type;

	/** Determines how the event can be deactivated. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, DisplayName="Deactivate When", meta=(EditCondition="Type==EBangoEventType::Toggle", EditConditionHides))
	EBangoToggleDeactivateCondition DeactivateCondition;
	
	/** When set, this event can only be triggered this many times before it becomes expired. Expired events will ignore any further trigger signals. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, meta=(EditCondition="bUseActivationLimit", ClampMin=1))
	int32 ActivationLimit = 1;

	UPROPERTY()
	bool bUseActivationLimit = true;

	/** Causes the event to activate & deactivate. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, Instanced, meta=(ShowInnerProperties))
	TObjectPtr<UBangoTrigger> Trigger;
	
	/** Actions for the event. */
	UPROPERTY(Category="Bango|Settings", EditAnywhere, Instanced, meta=(ShowInnerProperties))
	TArray<TObjectPtr<UBangoAction>> Actions;
	
	/** If true, the event will need to be unfrozen before it can be activated. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;
	
	// SETTINGS GETTERS AND SETTERS
	// ------------------------------------------
#if WITH_EDITORONLY_DATA
public:
	FText GetDisplayName() const;
	
	bool GetUsesCustomColor() const;
	
	FLinearColor GetCustomColor() const;
#endif
	
public:
	UFUNCTION(BlueprintCallable)
	bool GetStartsFrozen() const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerLimit() const;

	UFUNCTION(BlueprintCallable)
	void SetTriggerLimit(int32 NewTriggerLimit);
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerCount() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsToggleType() const;

	UFUNCTION(BlueprintCallable)
	bool IsInstancedType() const;

	bool IsBangType() const;
	
	UFUNCTION(BlueprintCallable)
	EBangoEventType GetType() const;

	UFUNCTION(BlueprintCallable)
	EBangoToggleDeactivateCondition GetDeactivateCondition() const;

	UFUNCTION(BlueprintCallable)
	const TArray<UBangoAction*>& GetActions() const;

	UFUNCTION(BlueprintCallable)
	bool GetUsesCustomMesh() const;
	
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
	
	/**  */
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	int32 DeactivationCount = 0;
	
	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	double LastActivationTime = -999;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	double LastDeactivationTime = -998;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventActivated OnBangoEventActivated;

	UPROPERTY(Category="Bango|State (Debug)", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventDeactivated OnBangoEventDeactivated;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UMaterialInstanceDynamic* CustomMaterialDynamic;
#endif
	// ------------------------------------------
	// STATE GETTERS
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool GetIsFrozen() const;

	UFUNCTION(BlueprintCallable)
	bool ActivationLimitReached() const;

	UFUNCTION(BlueprintCallable)
	bool DeactivationLimitReached() const;

	UFUNCTION(BlueprintCallable)
	double GetLastActivationTime() const;

	UFUNCTION(BlueprintCallable)
	double GetLastDeactivationTime() const;

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
	void Activate(UObject* ActivationInstigator);

	UFUNCTION(BlueprintCallable)
	void Deactivate(UObject* DeactivationInstigator);

protected:
	void EnableTrigger();

	void DisableTrigger();
	
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

	TObjectPtr<UStaticMeshComponent> OverrideDisplayMesh;
	
public:
	const FBangoEventStateFlag& GetState() const;
	
	bool HasCurrentState(EBangoEventState State);

	void Destroyed() override;
	
	void OnConstruction(const FTransform& Transform) override;

	void UpdateProxyState();

protected:
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont) const;
	
	double GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation) const;
	
	FCanvasTextItem GetDebugHeaderText(const FVector& ScreenLocationCentre) const;

	TArray<FCanvasTextItem> GetDebugDataText(UCanvas* Canvas, const FVector& ScreenLocationCentre, TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter) const;
	
	TArray<FBangoDebugTextEntry> GetDebugDataString_Editor() const;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Game() const;

	bool HasInvalidData() const;

public:
	FLinearColor GetColorForProxy() const;

protected:
	void OnCvarChange();
#endif
	
#if ENABLE_VISUAL_LOG
	void VLOG_Generic(FString Text, FColor Color, UObject* EventInstigator) const;
#endif
};










// TODO: types of trigger: contact, overlap, kill, global event message, 
// TODO: handle multiple simultaneous overlaps/triggers
// TODO: colorize colliders
// TODO: trigger from different sources - overlaps, impacts of other actors?, messages from other actors?
// TODO: editor icons or some way to distinguish
// TODO: trigger type selector? Have actions provide an icon/identifier?
// TODO: need some way to freeze/unfreeze an event? So that if you want to change the world... one event could unfreeze another event.
