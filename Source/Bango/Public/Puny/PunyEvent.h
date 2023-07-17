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

	UPROPERTY()
	FPunyInstigatorRecordCollection InstigatorRecords;
	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------

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
	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
	
	
	
};