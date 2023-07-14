// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Trigger/BangoTrigger.h"

#include "BangoTrigger_Bang.generated.h"

/** Optional "Bang" trigger, usable only on Bang events. */
UCLASS(Abstract, Within="BangoEvent_Bang")
class BANGO_API UBangoTrigger_Bang : public UBangoTrigger
{
	GENERATED_BODY()
};