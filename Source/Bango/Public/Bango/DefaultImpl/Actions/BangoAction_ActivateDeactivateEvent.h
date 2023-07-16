// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Action/BangoAction_Toggle.h"

#include "BangoAction_ActivateDeactivateEvent.generated.h"


UCLASS(DisplayName="Activate/Deactivate Event")
class BANGO_API UBangoAction_ActivateDeactivateEvent : public UBangoAction
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoAction_ActivateDeactivateEvent();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	
	UPROPERTY(EditInstanceOnly, Category="Settings")
	TSoftObjectPtr<ABangoEvent> TargetEvent;
	
	/** When our event responds to left signal, send target event the right signal. */
	UPROPERTY(EditAnywhere, Category="Settings");
	TMap<EBangoSignal, EBangoSignal> ActionSignalMap;
	
	// ============================================================================================
	// STATE
	// ============================================================================================

	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator) override;

private:
	void Execute(EBangoSignal Signal);
};
