// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "TriggerSignal.generated.h"

//enum class EBangoTriggerSignalType : uint8;

UENUM()
enum class EBangoTriggerSignalType : uint8
{
	None,
	ActivateEvent,
	DeactivateEvent,
	MAX
};

USTRUCT(BlueprintType)
struct FBangoTriggerSignal
{
	GENERATED_BODY()

	FBangoTriggerSignal();

	FBangoTriggerSignal(EBangoTriggerSignalType InType, UObject* InInstigator);
	
	EBangoTriggerSignalType Type;

	UObject* Instigator;
};
