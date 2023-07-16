#pragma once

#include "PunyEventSignalType.generated.h"

UENUM(BlueprintType)
enum class EPunyEventSignalType : uint8
{
	None,
	StartAction,
	StopAction,
	MAX
};