// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once
#include "Puny/Core/EventSignal.h"
#include "Puny/Core/InstigatorRecords.h"

#include "Event.generated.h"

enum class EPunyTriggerSignalType : uint8;
class UPunyEventComponent;
class UPunyAction;
struct FPunyTriggerSignal;
struct FBangoDebugTextEntry;

// TODO do I really need Dynamic
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPunyEventExpiredDelegate, UPunyEvent*, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPunyEventSignalDelegate, UPunyEvent*, Event, FPunyEventSignal, Signal);

UENUM(BlueprintType)
enum class EPunyExistingSignalHandling : uint8
{
	/**  */
	LetRun,
	
	/**  */
	Restart,
};

UENUM(BlueprintType)
enum class EPunyOpposingSignalHandling : uint8
{
	/** If a signal is pending delay and the opposite signal is sent, the first one will be immediately cancelled and the second signal will start as if the first never began. */
	CancelOpposingAndContinue,

	/** If a signal is pending delay and the opposite signal is sent, the first one will be immediately cancelled and the second signal will start as if the first never began. */
	CancelOpposing,

	/** Do nothing - let existing signals run and let this signal run. */
	IgnoreOpposing,

	/** If there is an existing signal undergoing delay, this signal will be discarded. */
	Exclusive,
};

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UPunyEvent : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
public:
	/**  */
	UPunyEvent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

private:
	/** Whether to use activate limit features or not. */
	UPROPERTY(Category="Settings", EditAnywhere)
	bool bUseTriggerLimits = false;

	/** Number of times this event can be activated. */
	UPROPERTY(Category="Settings", EditAnywhere, meta=(EditCondition="bUseTriggerLimits", EditConditionHides, UIMin = 1, UIMax = 10, DisplayPriority=-1))
	uint32 ActivateLimit = 1;
	
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere, meta=(EditCondition="bUseTriggerLimits", EditConditionHides, UIMin = 1, UIMax = 10, DisplayPriority=-1))
	uint32 DeactivateLimit = 1;
	
	/** Whether to use timed signal features or not. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(DisplayPriority=-1))
	bool bUseSignalDelays = false;

	/**  */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides, UIMin=0, ClampMin=0, UIMax=60, Units="s", Delta=0.5, DisplayPriority=-1))
	float ActivateDelay = 0.0f;

	/**  */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides, UIMin=0, ClampMin=0, UIMax=60, Units="s", Delta=0.5, DisplayPriority=-1))
	float DeactivateDelay = 0.0f;

	/** Determines what action to take if e.g. an Activate trigger occurs while an existing Activate trigger is already waiting on a delay. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides, DisplayPriority=-1))
	EPunyExistingSignalHandling ExistingSignalHandling = EPunyExistingSignalHandling::LetRun;	
	
	/** Determines what action to take if e.g. a Deactivate trigger occurs while an Activate trigger is waiting on a delay. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides, DisplayPriority=-1))
	EPunyOpposingSignalHandling OpposingSignalHandling = EPunyOpposingSignalHandling::CancelOpposingAndContinue;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

public:
	/**  */
	bool GetUsesActivateLimit();

	/**  */
	uint32 GetActivateLimit();

	/**  */
	uint32 GetDeactivateLimit();

	// ============================================================================================
	// STATE
	// ============================================================================================

protected:
	/**  */
	UPROPERTY()
	FPunyInstigatorRecordCollection InstigatorRecords;

private:
	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1, DisplayThumbnail=false))
	UObject* LastActivateInstigator = nullptr;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1, DisplayThumbnail=false))
	UObject* LastDeactivateInstigator = nullptr;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	double LastActivateTime = -1.0;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	double LastDeactivateTime = -1.0;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	uint32 ActivateCount = 0;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	uint32 DeactivateCount = 0;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	FTimerHandle DelayedActivateHandle;

	/**  */
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	FTimerHandle DelayedDeactivateHandle;
	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------

public:
	/**  */
	UObject* GetLastActivateInstigator();

	/**  */
	UObject* GetLastDeactivateInstigator();

	/**  */
	double GetLastActivateTime();

	/**  */
	double GetLastDeactivateTime();

	/**  */
	uint32 GetActivateCount();

	/**  */
	uint32 GetDeactivateCount();

	/**  */
	virtual bool GetIsExpired();
	
	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------
	
private:
	/**  */
	UPROPERTY(Transient)
	FPunyEventSignalDelegate EventSignal;

	/**  */
	UPROPERTY(Transient)
	FPunyEventExpiredDelegate ExpiryDelegate;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
	
public:
	/**  */
	virtual void Init();
	
public:
	/**  */
	void RegisterAction(UPunyAction* Action);

	/**  */
	void UnregisterAction(UPunyAction* Action);
	
	/**  */
	UFUNCTION()
	void RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal);

	/**  */
	void RespondToTriggerSignalDeferred(UPunyTrigger* Trigger, FPunyTriggerSignal Signal);

	/**  */
	virtual EPunyEventSignalType RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal);

protected:
	/**  */
	UFUNCTION()
	void AddInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType);
	
	/**  */
	UPunyEventComponent* GetEventComponent();

	/**  */
	AActor* GetActor();

	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================

#if WITH_EDITORONLY_DATA
public:
	/**  */
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> PunyEventsShowFlag;
#endif

	FSimpleDelegate OnStateChange;
	
	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

#if WITH_EDITOR
#endif

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

#if WITH_EDITORONLY_DATA
#endif
	
	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------

#if WITH_EDITOR
public:
	/**  */
	virtual FLinearColor GetDisplayBaseColor();

	/**  */
	virtual void ApplyColorEffects(FLinearColor& Color);
	
	/**  */
	virtual bool GetIsPlungerPushed();
#endif
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
	
#if WITH_EDITOR
	/**  */
	virtual void AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data);
#endif
};