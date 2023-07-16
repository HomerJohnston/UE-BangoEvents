// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Bango/Event/BangoEventState.h"
#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Event/BangoEventInstigation.h"
#include "BangoEvent.generated.h"

enum class EBangoSignal : uint8;
class UBangoEventProcessor;
class UObject;
class UBangoAction;
class UBangoTrigger;
class UBangoPlungerComponent;
class FCanvasTextItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBangoEventTriggered, ABangoEvent*, Event, EBangoSignal, Signal, UObject*, Instigator);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBangoEventSignalLimitReached, ABangoEvent*, Event, EBangoSignal, Signal);

/** Base class for events. Cannot be used as-is, all events must either be a Bang or a Toggle type (or some other custom-built type). */
UCLASS(Abstract)
class BANGO_API ABangoEvent : public AActor
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	ABangoEvent();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
	
#if WITH_EDITORONLY_DATA
protected:
	/** Set to override the editor display name. */
	UPROPERTY(Category="Bango|Display", DisplayName="Display Name Override", EditInstanceOnly, BlueprintReadOnly, meta=(EditCondition="bUseDisplayName"))
	FText DisplayName;

	UPROPERTY()
	bool bUseDisplayName = false;
	
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
	
	const float DebugTextOffsetBase = +100.0;
	const float DebugTextOffsetSizeScaler = +50.0;
#endif
	
protected:
	UPROPERTY(Category="Bango|Event", EditAnywhere, meta=(EditCondition="bUseTriggerLimit", UIMin = 1, UIMax = 10))
	int32 TriggerLimit = 1;

	UPROPERTY()
	bool bUseTriggerLimit = false;
	
	/** Triggers send signals to events (i.e. to activate or deactivate). */
	UPROPERTY(Category="Bango|Event", EditAnywhere, Instanced, meta=(ShowInnerProperties))
	TArray<TObjectPtr<UBangoTrigger>> Triggers;
	
	/** Actions for the event. */
	UPROPERTY(Category="Bango|Event", EditAnywhere, Instanced, meta=(ShowInnerProperties))
	TArray<TObjectPtr<UBangoAction>> Actions;

	/** If true, the event will need to be unfrozen before it can be activated. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;

	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bFreezeWhenAllTriggerLimitsReached = true;

	
	// ------------------------------------------
	// Settings Getters and Setters
	// ------------------------------------------
#if WITH_EDITORONLY_DATA
public:
	FText GetDisplayName() const;
	
	bool GetUsesCustomColor() const;
	
	FLinearColor GetCustomColor() const;
	
	bool GetUsesCustomMesh() const;
#endif
	
public:
	UFUNCTION(BlueprintCallable)
	bool GetStartsFrozen() const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerLimit() const;

	UFUNCTION(BlueprintCallable)
	void SetTriggerLimit(int32 NewTriggerLimit);
	
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerCount(EBangoSignal Signal) const;

	// TODO: test ResetEvent to fully reset state
	// ============================================================================================
	// STATE
	// ============================================================================================
public:
	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventTriggered OnEventTriggered;

	UPROPERTY(Category="Bango|Debug", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventTriggered OnEventSignalled;
	
	UPROPERTY(Category="Bango|Debug", Transient, BlueprintAssignable, BlueprintReadOnly, VisibleInstanceOnly)
	FOnBangoEventSignalLimitReached OnSignalLimitReached;
	
protected:
	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	TMap<EBangoSignal, int32> TriggerCounts;

	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	TMap<EBangoSignal, double> LastTriggerTimes;

	UPROPERTY(Category="Bango|Debug", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	TMap<EBangoSignal, UObject*> LastInstigators;
	
	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	TMap<EBangoSignal, FBangoEventInstigationArray> InstigatorData;

	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bFrozen = false;

	UPROPERTY(Category="Bango|Debug", Transient, BlueprintReadOnly, VisibleInstanceOnly)
	TArray<EBangoSignal> RemainingTriggerLimits;

#if WITH_EDITORONLY_DATA
protected:
	
	UPROPERTY()
	UMaterialInstanceDynamic* CustomMaterialDynamic;

	/** Manually send a trigger signal when debug button is pressed. */
	UPROPERTY(Category="Bango", Transient, BlueprintReadOnly, EditInstanceOnly)
	EBangoSignal DebugSignal;

	/** If not set, will use the event (self) as instigator. */
	UPROPERTY(Category="Bango", Transient, BlueprintReadOnly, EditInstanceOnly)
	AActor* DebugSignalInstigator = nullptr;
	
	UFUNCTION(Category="Bango", CallInEditor)
	void TriggerDebugSignal();
#endif
	
	// ------------------------------------------
	// State Getters
	// ------------------------------------------
public:
	
	UFUNCTION(BlueprintCallable)
	bool TriggerLimitReached(EBangoSignal Signal) const;

	UFUNCTION(BlueprintCallable)
	double GetLastTriggerTime(EBangoSignal Signal) const;

	UFUNCTION(BlueprintCallable)
	const UObject* GetLastInstigator(EBangoSignal Signal) const;
	
	UFUNCTION(BlueprintCallable)
	bool GetIsFrozen() const;
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	UFUNCTION(BlueprintCallable)
	void Trigger(EBangoSignal Signal, UObject* NewInstigator);

	UFUNCTION(BlueprintCallable)
	void ResetEvent(bool bUnfreeze = false);

	UFUNCTION(BlueprintCallable)
	virtual void SetFrozen(bool bNewFrozen);
	
protected:
	void BeginPlay() override;

	virtual bool ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator);

protected:
	virtual void ResetRemainingTriggerLimits();
	
	void CleanupInvalidTriggers();
	
	void CleanupInvalidActions();
	
	void SetTriggers(bool bNewEnabled);

#if WITH_EDITOR
public:
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> BangoEventsShowFlag;

protected:
	FBangoEventStateFlag CurrentState;

	FDelegateHandle DebugDrawService_Editor;
	
	FDelegateHandle DebugDrawService_Game;

	TObjectPtr<UBangoPlungerComponent> PlungerComponent;

	TObjectPtr<UStaticMeshComponent> OverrideDisplayMesh;

	virtual FLinearColor GetColorBase() const;
	
public:
	const FBangoEventStateFlag& GetState() const;
	
	bool HasCurrentState(EBangoEventState State);

	void Destroyed() override;
	
	void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void UpdateProxyState();

	void DebugDraw(UCanvas* Canvas, APlayerController* PlayerController) const;
	
	bool GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation, double& DistSqrd) const;
	
	FCanvasTextItem GetDebugHeaderText(const FVector& ScreenLocationCentre, double Distance) const;

	TArray<FCanvasTextItem> GetDebugDataText(UCanvas* Canvas, const FVector& ScreenLocationCentre, TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter, double Distance) const;
	
	virtual TArray<FBangoDebugTextEntry> GetDebugDataString_Editor() const;

	virtual TArray<FBangoDebugTextEntry> GetDebugDataString_Game() const;

	virtual bool HasInvalidData() const;

public:
	virtual FLinearColor GetColorForProxy() const;

protected:
	void OnCvarChange();
#endif
	
#if ENABLE_VISUAL_LOG
	void VLOG_Generic(FString Text, FColor Color, UObject* LogInstigator = nullptr) const;
#endif
};

// TODO: types of trigger: contact, overlap, kill, global event message, 
// TODO: handle multiple simultaneous overlaps/triggers
// TODO: colorize colliders
// TODO: trigger from different sources - overlaps, impacts of other actors?, messages from other actors?
// TODO: editor icons or some way to distinguish
// TODO: trigger type selector? Have actions provide an icon/identifier?
// TODO: need some way to freeze/unfreeze an event? So that if you want to change the world... one event could unfreeze another event.
