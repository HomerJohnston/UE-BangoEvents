// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Trigger/BangoTrigger.h"

#include "BangoTrigger_Toggle.generated.h"

/** Optional "Toggle" trigger, usable only on Toggle events. */
UCLASS(Abstract, Within="BangoEvent_Toggle")
class BANGO_API UBangoTrigger_Toggle : public UBangoTrigger
{
	GENERATED_BODY()
};