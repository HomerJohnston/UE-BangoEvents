#pragma once
#include "Puny/PunyEventSignal.h"
#include "Puny/PunyInstigatorRecords.h"

#include "PunyEvent.generated.h"

class UPunyEventComponent;
class UPunyAction;
struct FPunyTriggerSignal;

// TODO do I really need Dynamic
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPunyEventExpiredDelegate, UPunyEvent*, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPunyEventSignalDelegate, UPunyEvent*, Event, FPunyEventSignal, Signal);

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UPunyEvent : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UPunyEvent();
	 
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
private:
	/**  */
	UPROPERTY()
	bool bUseActivateLimit = false;

	/**  */
	UPROPERTY(Category="Settings", EditAnywhere, meta=(EditCondition="bUseActivateLimit", UIMin = 1, UIMax = 10, DisplayPriority=-1))
	uint32 ActivateLimit = 1;

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------
public:
	bool GetUsesActivateLimit();

	uint32 GetActivateLimit();

	bool GetIsForceDisabled();
		
	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	UPROPERTY()
	FPunyInstigatorRecordCollection InstigatorRecords;

private:
	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1, DisplayThumbnail=false))
	UObject* LastActivateInstigator = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1, DisplayThumbnail=false))
	UObject* LastDeactivateInstigator = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	double LastActivateTime = -1.0;

	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	double LastDeactivateTime = -1.0;

	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	uint32 ActivateCount = 0;

	UPROPERTY(VisibleAnywhere, Category="Debug", meta=(DisplayPriority=-1))
	uint32 DeactivateCount = 0;

	bool bFrozen = false;
	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------
public:
	
	UObject* GetLastActivateInstigator();

	UObject* GetLastDeactivateInstigator();

	double GetLastActivateTime();

	double GetLastDeactivateTime();

	uint32 GetActivateCount();

	uint32 GetDeactivateCount();

	virtual bool GetIsExpired();
	
	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------
protected:
	UPROPERTY(Transient)
	FPunyEventSignalDelegate EventSignal;

	UPROPERTY(Transient)
	FPunyEventExpiredDelegate ExpiryDelegate;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	virtual void Init();
	
public:
	void RegisterAction(UPunyAction* Action);

	void UnregisterAction(UPunyAction* Action);
	
	UFUNCTION()
	void RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal);

	virtual EPunyEventSignalType RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal);

protected:

	UFUNCTION()
	void AddInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType);
	
	UPunyEventComponent* GetEventComponent();

	AActor* GetActor();

#if WITH_EDITORONLY_DATA
	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================
public:
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> PunyEventsShowFlag;
	
	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------
public:
	virtual FLinearColor GetDisplayBaseColor();

	virtual void ApplyColorEffects(FLinearColor& Color);
	
	virtual bool GetIsPlungerPushed();
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================\
	
#endif
};