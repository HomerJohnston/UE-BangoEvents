// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoTriggerSignal.generated.h"

UENUM()
enum class EBangoTriggerSignalType : uint8
{
	DoNothing,
	ActivateEvent,
	DeactivateEvent,
	MAX
};

USTRUCT(BlueprintType)
struct BANGO_API FBangoTriggerSignal
{
	GENERATED_BODY()

	FBangoTriggerSignal();

	FBangoTriggerSignal(EBangoTriggerSignalType InType, UObject* InInstigator);
	
	EBangoTriggerSignalType Type;

	UObject* Instigator;
};
