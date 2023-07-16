#pragma once

#include "PunyEventSignal.generated.h"

enum class EPunyEventSignalType : uint8;

USTRUCT(BlueprintType)
struct FPunyEventSignal
{
	GENERATED_BODY()

	FPunyEventSignal();

	FPunyEventSignal(EPunyEventSignalType InType, UObject* InInstigator);
	
	EPunyEventSignalType Type;

	UObject* Instigator;
};
