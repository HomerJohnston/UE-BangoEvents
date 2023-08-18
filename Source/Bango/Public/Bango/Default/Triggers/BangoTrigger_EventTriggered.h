// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/BangoTrigger.h"

#include "BangoTrigger_EventTriggered.generated.h"

class UBangoEventComponent;

UENUM()
enum class EBangoEventSignalledRequirement : uint8
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

UCLASS(DisplayName="Event Triggered")
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
	EBangoEventSignalledRequirement Requirement;

	/** What event(s) to watch? */ // TODO: I should have an FComponentReference property to pick actual events. But FCompRef is bugged and cannot reference non-CDO components. As a weak workaround, pick whole actors and find all components manually.
	UPROPERTY(EditInstanceOnly, Category="Settings")
	TArray<TSoftObjectPtr<AActor>> MasterEventSources;

	UPROPERTY(VisibleInstanceOnly, Category="Settings", Transient)
	TArray<TSoftObjectPtr<UBangoEventComponent>> MasterEvents;
	
	/** If true, this event will be activated as if the watched event's instigator had triggered us directly. If false, will use this trigger object as our instigator. */
	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="Use Master Event's Instigator");
	bool bUseInstigatorFromMasterEvent;

	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="bOverrideSignalMapping", EditConditionHides))
	EBangoTriggerSignalType OnTargetEventActivated;

	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(EditCondition="bOverrideSignalMapping", EditConditionHides))
	EBangoTriggerSignalType OnTargetEventDeactivated;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	UPROPERTY()
	TArray<EBangoEventSignalType> MasterEventsRecentSignals;
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

private:
	UFUNCTION()
	void OnTargetEventSignalled(UBangoEventComponent* EventComponent, EBangoEventSignalType EventSignal, UObject* SignalInstigator);

	// ============================================================================================
	// EDITOR
	// ============================================================================================

	void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;
};