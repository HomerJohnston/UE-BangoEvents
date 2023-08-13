// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "EventSignal.generated.h"

UENUM()
enum class EBangoEventSignalType : uint8
{
	None,
	ActivateEvent,
	DeactivateEvent,
	MAX
};

USTRUCT(BlueprintType)
struct FBangoEventSignal
{
	GENERATED_BODY()

	FBangoEventSignal();

	FBangoEventSignal(EBangoEventSignalType InType, UObject* InInstigator);
	
	EBangoEventSignalType Type;

	UObject* Instigator;
};
