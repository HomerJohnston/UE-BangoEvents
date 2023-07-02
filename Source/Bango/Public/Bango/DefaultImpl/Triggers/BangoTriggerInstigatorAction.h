#pragma once

#include "BangoTriggerInstigatorAction.generated.h"

UENUM(BlueprintType)
enum class EBangoTriggerInstigatorAction : uint8
{
	DoNothing,
	AddInstigator,
	RemoveInstigator,
};