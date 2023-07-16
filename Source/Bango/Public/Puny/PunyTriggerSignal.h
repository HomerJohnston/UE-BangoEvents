#pragma once

#include "PunyTriggerSignal.generated.h"

enum class EPunyTriggerSignalType : uint8;

USTRUCT(BlueprintType)
struct FPunyTriggerSignal
{
	GENERATED_BODY()

	FPunyTriggerSignal();

	FPunyTriggerSignal(EPunyTriggerSignalType InType, UObject* InInstigator);
	
	EPunyTriggerSignalType Type;

	UObject* Instigator;
};
