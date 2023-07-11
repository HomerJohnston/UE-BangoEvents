// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Action/BangoAction.h"

#include "BangoAction_DebugLog.generated.h"

class ALight;

UCLASS(DisplayName="Debug Log")
class UBangoAction_DebugLog : public UBangoAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category="Settings", EditAnywhere)
	TMap<EBangoSignal, FString> SignalMessageMap;
	
public:
	void ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator) override;

protected:
	FText GetEventName();
	
	FString GetInstigatorName();
	
	// ============================================================================================
	// Editor
	// ============================================================================================
};
