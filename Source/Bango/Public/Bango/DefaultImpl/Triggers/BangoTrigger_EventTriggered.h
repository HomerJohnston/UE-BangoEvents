// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Trigger/BangoTrigger.h"

#include "BangoTrigger_EventTriggered.generated.h"

enum class EBangoActivateDeactivateEventAction : uint8;

UENUM()
enum class EBangoEventTriggeredRequirement : uint8
{
	/** No effect. */
	None,
	/** If any master events are signalled, this trigger will fire using just that master's signal. */
	AnyChanged,
	/** If all master events become the same signal, this trigger will fire using the common signal. */
	AllChanged,
	/** All master events must activate for this trigger to send an activate signal. Any master event deactivating will cause this trigger to send a deactivate signal. */
	AllActivateAnyDeactivate,
	/** Any master event activating will cause this trigger to send an activate signal. All master events must deactivate for this trigger to send a deactivate signal. */
	AnyActivateAllDeactivate,

	Max
};

UCLASS(DisplayName="Other Event Triggered")
class BANGO_API UBangoTrigger_EventTriggered : public UBangoTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_EventTriggered();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	/** Should this trigger fire when any master events activate, or only when all activate? */
	UPROPERTY(EditAnywhere, Category="Settings")
	EBangoEventTriggeredRequirement Requirement;

	/** If false, this trigger will watch its own event. This is useful to program simple cooldown-style effects. */
	UPROPERTY(EditAnywhere, Category="Settings")
	bool bWatchOtherEvents = false;
	
	/** What event(s) to watch? */
	UPROPERTY(EditInstanceOnly, Category="Settings", meta=(EditCondition="bWatchOtherEvents", EditConditionHides))
	TArray<TSoftObjectPtr<ABangoEvent>> MasterEvents;
	
	/** If true, this event will be activated as if the watched event's instigator had triggered us directly. If false, will use this trigger object as our instigator. */
	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="Use Master Event's Instigator");
	bool bUseMasterEventInstigator;

	/** All signals triggering the master event are simply copied to trigger our event. If we enable this, we can use a mapping to translate the master event's signals into different signals for our event. */
	UPROPERTY(EditAnywhere, Category="Settings")
	bool bOverrideSignalMapping = false;
	
	/** When the triggered with the master's left signal, we will trigger our event with the right signal. */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="bOverrideSignalMapping", EditConditionHides, ReadOnlyKeys));
	TMap<EBangoSignal, EBangoSignal> SignalMapping;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	UPROPERTY()
	TArray<EBangoSignal> MasterEventMostRecentSignals;
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

private:
	UFUNCTION()
	void OnTargetEventTriggered(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator);
};