#pragma once

#include "PunyTriggerSignalType.generated.h"

UENUM()
enum class EPunyTriggerSignalType : uint8
{
	None,
	ActivateEvent,
	DeactivateEvent,
	MAX
};
