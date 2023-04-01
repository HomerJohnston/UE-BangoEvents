#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "BangoEvent.generated.h"

class UObject;
class UBangoAction;
class UBangoTriggerCondition;
class UBangoPlungerComponent;

UENUM()
enum class EBangoWorldTimeType : uint8
{
	GameTime,
	UnpausedTime,
	RealTime,
	AudioTime,
	MAX
};

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor))
enum class EBangoEventState : uint8
{
	Normal,
	Active,
	Frozen,
	Expired,
	StartDelay,
	MAX
};

USTRUCT()
struct FBangoRunStateSettings
{
	GENERATED_BODY()
	
	/** When on, start and stop actions will be run any time an instigator triggers or stops triggering this event.
	 * When off, start actions will only be run when the first instigator triggers this event, and stop actions will only be run when all instigators are removed.*/
	UPROPERTY(Category="Bango|Run Settings", EditAnywhere)
	bool bRunForEveryInstigator = false;
	
	/** If true, 2nd and 3rd... instigators will not cause any actions to run. If the first instigator is removed, actions will then be run on the second instigator. */
	UPROPERTY(Category="Bango|Run Settings", EditAnywhere, meta=(EditCondition="bRunForEveryInstigator"))
	bool bQueueInstigators = false;
};

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
	/**  */
	UPROPERTY(Category="Bango", EditAnywhere)
	bool bStartsAndStops = false;
	
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
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bStartsAndStops", EditConditionHides))
	FBangoRunStateSettings RunStateSettings;
	
	/**  */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoTriggerCondition*> StartTriggers;
	
	/** Actions to run when event is triggered, or turns on for an on/off event. */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoAction*> StartActions;
	
	/**  */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced, meta=(EditCondition="bStartsAndStops", EditConditionHides))
	TArray<UBangoTriggerCondition*> StopTriggers;

	/** Actions to run when an on/off event turns off. */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced, meta=(EditCondition="bStartsAndStops", EditConditionHides))
	TArray<UBangoAction*> StopActions;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	FName DisplayName;
#endif
	
	/**  */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;

	/** When set, freezes the event after it expires and runs out of instigators, typically to disable triggers and save CPU. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere, meta=(EditCondition="bUseTriggerLimit", HideEditConditionToggle, EditConditionHides))
	bool bFreezeWhenExpired = true;
	
	/** How to measure trigger delay times or trigger hold times. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	EBangoWorldTimeType TimeType = EBangoWorldTimeType::GameTime;
	
	/** Any events specified will be triggered whenever this event is triggered, using identical instigator data. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditInstanceOnly)
	TArray<ABangoEvent*> SlavedEvents;
	
	// ------------------------------------------
	// Settings Getters
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	int32 GetTriggerLimit();

	UFUNCTION(BlueprintCallable)
	int32 GetTriggerCount();
	
	UFUNCTION(BlueprintCallable)
	double GetStartTriggerDelay();
	
	UFUNCTION(BlueprintCallable)
	double GetStopTriggerDelay();
	
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
	
	// ------------------------------------------
	// State Getters
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool GetIsFrozen();

	UFUNCTION(BlueprintCallable)
	bool GetIsExpired();
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void BeginPlay() override;

	void ResetTriggerCount(bool bUnfreeze = true);

protected:
	UFUNCTION()
	void ActivateFromTrigger(UObject* NewInstigator);

	UFUNCTION()
	void DeactivateFromTrigger(UObject* OldInstigator);

	UFUNCTION(BlueprintCallable)
	void SetFrozen(bool bNewFrozen);

	void EnableTriggers(TArray<UBangoTriggerCondition*>& Triggers);

	void DisableTriggers(TArray<UBangoTriggerCondition*>& Triggers);
	
	void Update();

	void RunActions(UObject* NewInstigator, TArray<UBangoAction*>& Actions);
	
	// ============================================================================================
	// Editor !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ============================================================================================
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(Transient)
	int NumCollisionVolumes = 0;

	UPROPERTY(Transient)
	TSet<EBangoEventState> CurrentStates;

	UPROPERTY(Transient)
	int NumStopTriggers = 0;

	UPROPERTY(Transient)
	int bRunForEveryInstigatorSet = 0;

	UPROPERTY(Transient)
	int bUseTriggerLimitSet = 0;
	
	UPROPERTY(Transient)
	TObjectPtr<UBangoPlungerComponent> PlungerComponent;
#endif

#if WITH_EDITOR
public:
	bool HasCurrentState(EBangoEventState State);
	
	void OnConstruction(const FTransform& Transform) override;

	void BeginDestroy() override;
	
	void DebugUpdate();
	
	void UpdateState();

	bool CanEditChange(const FProperty* InProperty) const override;
	
private:
	void UpdateEditorVars();

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	
#endif
};










// TODO: types of trigger: contact, overlap, kill, global event message, 
// TODO: handle multiple simultaneous overlaps/triggers
// TODO: colorize colliders
// TODO: trigger from different sources - overlaps, impacts of other actors?, messages from other actors?
// TODO: editor icons or some way to distinguish
// TODO: trigger type selector? Have actions provide an icon/identifier?
// TODO: need some way to freeze/unfreeze an event? So that if you want to change the world... one event could unfreeze another event.
