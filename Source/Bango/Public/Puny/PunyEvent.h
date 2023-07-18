#pragma once
#include "Puny/PunyEventSignal.h"
#include "Puny/PunyInstigatorRecords.h"

#include "PunyEvent.generated.h"

class UPunyEventComponent;
class UPunyAction;
struct FPunyTriggerSignal;


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

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	UPROPERTY()
	FPunyInstigatorRecordCollection InstigatorRecords;

private:
	// TODO can this be editor only?
	// TODO \/
	UObject* LastActivateInstigator = nullptr;

	UObject* LastDeactivateInstigator = nullptr;

	double LastActivateTime = -1.0;

	double LastDeactivateTime = -1.0;
	// TODO /\
	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------
public:
	
	UObject* GetLastActivateInstigator();

	UObject* GetLastDeactivateInstigator();

	double GetLastActivateTime();

	double GetLastDeactivateTime();
	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------
protected:
	FPunyEventSignalDelegate EventSignal;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	virtual void Init();
	
public:
	void RegisterAction(UPunyAction* Action);

	void UnregisterAction(UPunyAction* Action);
	
	UFUNCTION()
	virtual void RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal);

protected:

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
	virtual FLinearColor GetDisplayColor();
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
	
	
#endif
};