// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Trigger/BangoTrigger.h"

#include "BangoTrigger_EventSignalled.generated.h"

enum class EBangoActivateDeactivateEventAction : uint8;

UCLASS(DisplayName="Other Event Signalled")
class BANGO_API UBangoTrigger_EventSignalled : public UBangoTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_EventSignalled();
	
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
	
	/** When watched event responds to left signal, send our event the right signal. */
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