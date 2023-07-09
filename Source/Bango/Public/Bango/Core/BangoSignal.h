#pragma once

#include "BangoSignal.generated.h"

UENUM(BlueprintType)
enum class EBangoSignal : uint8
{
	None,
	Activate,
	Deactivate,
	MAX			UMETA(Hidden)
};