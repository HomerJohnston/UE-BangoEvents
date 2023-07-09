﻿#pragma once

#include "Bango/Action/BangoToggleAction.h"
#include "Bango/Core/BangoInterfaces.h"

#include "BangoAction_FreezeThawEvent.generated.h"

enum class EBangoFreezeThawEventAction : uint8;

UCLASS(DisplayName="Freeze/Thaw Event")
class BANGO_API UBangoAction_FreezeThawEvent : public UBangoToggleAction, public IBangoToggleEventActionInterface
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

	UPROPERTY(Category="Settings", EditAnywhere, DisplayName = "On Start")
	EBangoFreezeThawEventAction OnStartAction;

	UPROPERTY(Category="Settings", EditAnywhere, DisplayName = "On Stop")
	EBangoFreezeThawEventAction OnStopAction;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void OnStart_Implementation() override;

	void OnStop_Implementation() override;

private:
	void Execute(EBangoFreezeThawEventAction Type);
};
