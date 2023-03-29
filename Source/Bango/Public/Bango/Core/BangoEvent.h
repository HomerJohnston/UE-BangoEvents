#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "BangoEvent.generated.h"

class UObject;
class UBangoAction;
class UBangoTriggerCondition;

UENUM()
enum class EBangoWorldTimeType : uint8
{
	GameTime,
	AudioTime,
	RealTime,
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

UCLASS()
class BANGO_API ABangoEvent : public AActor
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category="Bango", EditAnywhere)
	FName EventName;
#endif
	
	// Settings ---------------------------------
protected:
	UPROPERTY()
	bool bUseTriggerCountLimit = true;
	
	// TODO: need some ability to reset the event so it can be triggered another group of times
	/** When set, this event can only be triggered this many times before it becomes expired. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bUseTriggerCountLimit"/*, EditConditionHides*/, ClampMin=1))
	int TriggerLimit = 1;

	UPROPERTY()
	bool bUseStartTriggerDelay = false;
	
	/** When set, activation will only occur after the specified length of time. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bUseStartTriggerDelay"/*, EditConditionHides*/, ClampMin = 0.0))
	double StartTriggerDelay = 0;

	// TODO: Implement stop delay? Get rid of start delay? 
//	UPROPERTY()
//	bool bUseStopTriggerDelay = false;

	/** When set, deactivation will only occur after the specified length of time. */
//	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="bUseStopTriggerDelay"/*, EditConditionHides*/, ClampMin = 0.0))
//	double StopTriggerDelay = 0;
	
	/**  */ // TODO: should this be a list?
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoTriggerCondition*> StartTriggers;

	/**  */ // TODO: should this be a list?
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoTriggerCondition*> StopTriggers;

	/** When set, this event can be triggered multiple simultaneous times by different instigators. Useful for things like applying damage or healing to multiple instigators. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="NumStopTriggers > 0", EditConditionHides))
	bool bRunForEveryInstigator = false;
	
	/** If false, a deactivation trigger can only be received by the same instigator that activated this event. */
	UPROPERTY(Category="Bango", EditAnywhere, meta=(EditCondition="NumStopTriggers > 0 && bRunForEveryInstigatorSet == 0", EditConditionHides))
	bool bStopFromAnyInstigator = false;
	
	/** Actions to run when event is triggered, or turns on for an on/off event. */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced)
	TArray<UBangoAction*> StartActions;

	/** Actions to run when an on/off event turns off. */
	UPROPERTY(Category="Bango", EditAnywhere, Instanced, meta=(EditCondition="NumStopTriggers > 0", EditConditionHides))
	TArray<UBangoAction*> StopActions;

	/**  */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	bool bStartsFrozen = false;

	/** How to measure trigger delay times or trigger hold times. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditAnywhere)
	EBangoWorldTimeType TimeType = EBangoWorldTimeType::GameTime;
	
	/** Any events specified will be triggered whenever this event is triggered, using identical instigator data. */
	UPROPERTY(Category="Bango", AdvancedDisplay, EditInstanceOnly)
	TArray<ABangoEvent*> SlavedEvents;
	
	// State ------------------------------------
protected:
	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	bool bFrozen;

	/**  */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	int32 TriggerCount;
	
	/** Instigators which are actively triggering an on/off event. */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	TSet<UObject*> ActiveInstigators;

	/** Instigators which are going to be active, but are waiting for StartTriggerDelay to expire. */
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	TSet<UObject*> DelayedInstigators;
	
	UPROPERTY(Category="Bango|Debug", Transient, VisibleInstanceOnly)
	TMap<UObject*, FTimerHandle>  DelayedTimers;

	// API --------------------------------------
public:
	ABangoEvent();
	
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void Activate(UObject* NewInstigator);

	UFUNCTION()
	void Deactivate(UObject* OldInstigator);

protected:
	void AddActiveInstigator(UObject* NewInstigator);

	void RemoveActiveInstigator(UObject* OldInstigator);

	void AddDelayedInstigator(UObject* NewInstigator);

	void RemoveDelayedInstigator(UObject* OldInstigator);
	
	void FinishTriggerDelay(UObject* PendingInstigator);
	
	void Update();

	void Freeze();
	
	// Editor -----------------------------------
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(Transient)
	int NumCollisionVolumes = 0;

	UPROPERTY(Transient)
	UStaticMeshComponent* DebugMesh;

	UPROPERTY(Transient)
	TSet<EBangoEventState> CurrentStates;

	UPROPERTY(Transient)
	int NumStopTriggers = 0;

	UPROPERTY(Transient)
	int bRunForEveryInstigatorSet = 0;
#endif

#if WITH_EDITOR
public:
	bool HasCurrentState(EBangoEventState State);
	
	void OnConstruction(const FTransform& Transform) override;

	void BeginDestroy() override;
	
	void DebugUpdate();
	
	void UpdateState();
	
private:
	void UpdateEditorVars();

public:
	void SetDebugMesh(UStaticMesh* Mesh);

	void SetDebugMeshMaterial(UMaterialInstance* MaterialInstance);
	
	bool CanEditChange(const FProperty* InProperty) const override;
	
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
