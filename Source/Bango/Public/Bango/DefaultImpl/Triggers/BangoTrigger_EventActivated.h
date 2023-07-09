#pragma once

#include "Bango/Trigger/BangoTrigger.h"

#include "BangoTrigger_EventActivated.generated.h"

enum class EBangoActivateDeactivateEventAction : uint8;

UCLASS(DisplayName="Event Activated")
class BANGO_API UBangoTrigger_EventActivated : public UBangoTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_EventActivated();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:

	/** What event to watch? */
	UPROPERTY(EditInstanceOnly, Category="Settings")
	TSoftObjectPtr<ABangoEvent> WatchedEvent;

	/** If true, this event will be activated as if the watched event's instigator had triggered us directly. If false, will use this trigger object as our instigator. */
	UPROPERTY(EditAnywhere, Category="Settings");
	bool bUseWatchedEventInstigator;
	
	/** When watched event (left) is signalled, emit specified signal to our event (right). */
	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="OnStop");
	TMap<EBangoSignal, EBangoSignal> ActionSignalMap;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

private:
	UFUNCTION()
	void OnTargetEventSignalled(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator);
};