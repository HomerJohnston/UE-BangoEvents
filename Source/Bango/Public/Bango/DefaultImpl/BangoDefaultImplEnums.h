#pragma once

#include "BangoDefaultImplEnums.generated.h"

UENUM(BlueprintType)
enum class EBangoTriggerInstigatorAction : uint8
{
	AddInstigator,
	RemoveInstigator,
	DoNothing,
};

UENUM()
enum class EBangoFreezeThawEventAction : uint8
{
	FreezeEvent,
	UnfreezeEvent,
	DoNothing,
};