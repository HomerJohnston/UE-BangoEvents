// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoEvent.h"

#include "BangoEvent_Bang.generated.h"

class UBangoTrigger_Bang;
class UBangoAction_Bang;

UCLASS(DisplayName="Bango - Bang Event")
class BANGO_API ABangoEvent_Bang : public ABangoEvent
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	ABangoEvent_Bang();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	// ============================================================================================
	// STATE
	// ============================================================================================

	// ============================================================================================
	// API
	// ============================================================================================
public:
	virtual bool ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator) override;

protected:
	virtual bool HasInvalidData() const override;

protected:
	bool Activate(UObject* ActivateInstigator);

	void StartActions(UObject* StartInstigator);
	
	virtual void ResetRemainingTriggerLimits() override;

public:
#if WITH_EDITOR
public:
	void UpdateProxyState() override;

	FLinearColor GetColorBase() const override;

	FLinearColor GetColorForProxy() const override;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Game() const override;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Editor() const override;
#endif
};