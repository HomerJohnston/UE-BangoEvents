// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once
#include "Bango/Core/BangoEventSignal.h"
#include "Bango/Core/BangoInstigatorRecords.h"

#include "BangoEvent.generated.h"

enum class EBangoTriggerSignalType : uint8;
class UBangoEventComponent;
class UBangoAction;
struct FBangoDebugTextEntry;

// TODO do I really need Dynamic
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBangoEventExpiredDelegate, UBangoEvent*, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBangoEventSignalDelegate, UBangoEvent*, Event, FBangoEventSignal, Signal);

UENUM(BlueprintType)
enum class EBangoExistingSignalHandling : uint8
{
	/**  */
	LetRun,
	
	/**  */
	Restart,
};

UENUM(BlueprintType)
enum class EBangoOpposingSignalHandling : uint8
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
class BANGO_API UBangoEvent : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
public:
	/**  */
	UBangoEvent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

private:
	/** Whether to use activate limit features or not. */
	UPROPERTY(Category="Settings", EditAnywhere, meta=(DisplayPriority=-1))
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
	EBangoExistingSignalHandling ExistingSignalHandling = EBangoExistingSignalHandling::LetRun;	
	
	/** Determines what action to take if e.g. a Deactivate trigger occurs while an Activate trigger is waiting on a delay. */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(EditCondition="bUseSignalDelays", EditConditionHides, DisplayPriority=-1))
	EBangoOpposingSignalHandling OpposingSignalHandling = EBangoOpposingSignalHandling::CancelOpposingAndContinue;
	
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
	FBangoInstigatorRecordCollection InstigatorRecords;

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
	FBangoEventSignalDelegate EventTriggeredDelegate;

	/**  */
	UPROPERTY(Transient)
	FBangoEventExpiredDelegate ExpiryDelegate;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
	
public:
	/**  */
	virtual void Init();
	
public:
	/**  */
	void RegisterAction(UBangoAction* Action);

	/**  */
	void UnregisterAction(UBangoAction* Action);
	
	/**  */
	UFUNCTION()
	void RespondToTriggerSignal(UBangoTrigger* Trigger, FBangoTriggerSignal Signal);

	/**  */
	void RespondToTriggerSignalDeferred(UBangoTrigger* Trigger, FBangoTriggerSignal Signal);

	/**  */
	virtual EBangoEventSignalType RespondToTriggerSignal_Impl(UBangoTrigger* Trigger, FBangoTriggerSignal Signal);

protected:
	/**  */
	UFUNCTION()
	void AddInstigatorRecord(UObject* Instigator, EBangoEventSignalType SignalType);
	
	/**  */
	UBangoEventComponent* GetEventComponent();

	/**  */
	AActor* GetActor();

	virtual bool ShouldRespondToTrigger(EBangoTriggerSignalType TriggerSignalType);
	
	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================

#if WITH_EDITORONLY_DATA
public:
	/**  */
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> BangoEventsShowFlag;
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
	virtual bool GetIsActive();
#endif
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
	
#if WITH_EDITOR
	/**  */
	virtual void AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data);
#endif
};