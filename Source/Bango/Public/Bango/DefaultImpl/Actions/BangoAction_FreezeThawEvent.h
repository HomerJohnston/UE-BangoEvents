// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Action/BangoAction_Toggle.h"
#include "Bango/Action/BangoAction_Bang.h"

#include "BangoAction_FreezeThawEvent.generated.h"

UENUM()
enum class EBangoFreezeThawEventAction : uint8
{
	FreezeEvent,
	UnfreezeEvent,
	DoNothing,
};

UCLASS(DisplayName="Freeze/Thaw Event")
class BANGO_API UBangoAction_FreezeThawEvent : public UBangoAction
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoAction_FreezeThawEvent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	UPROPERTY(Category="Settings", EditInstanceOnly)
	TSoftObjectPtr<ABangoEvent> TargetEvent;

	UPROPERTY(Category="Settings", EditAnywhere, meta = (ReadOnlyKeys))
	TMap<EBangoSignal, EBangoFreezeThawEventAction> SignalActions;
		
	// ============================================================================================
	// STATE
	// ============================================================================================
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator) override;

private:
	void Execute(EBangoFreezeThawEventAction Type);
};
