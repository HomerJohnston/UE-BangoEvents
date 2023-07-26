// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "EventSignal.generated.h"

UENUM(BlueprintType)
enum class EPunyEventSignalType : uint8
{
	None,
	StartAction,
	StopAction,
	MAX
};

USTRUCT(BlueprintType)
struct FPunyEventSignal
{
	GENERATED_BODY()

	FPunyEventSignal();

	FPunyEventSignal(EPunyEventSignalType InType, UObject* InInstigator);
	
	EPunyEventSignalType Type;

	UObject* Instigator;
};
