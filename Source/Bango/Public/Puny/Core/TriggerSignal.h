// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "TriggerSignal.generated.h"

//enum class EPunyTriggerSignalType : uint8;

UENUM()
enum class EPunyTriggerSignalType : uint8
{
	None,
	ActivateEvent,
	DeactivateEvent,
	MAX
};

USTRUCT(BlueprintType)
struct FPunyTriggerSignal
{
	GENERATED_BODY()

	FPunyTriggerSignal();

	FPunyTriggerSignal(EPunyTriggerSignalType InType, UObject* InInstigator);
	
	EPunyTriggerSignalType Type;

	UObject* Instigator;
};
