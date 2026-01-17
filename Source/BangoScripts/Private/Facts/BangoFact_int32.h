#pragma once

#include "BangoScripts/Facts/BangoFactBase.h"
#include "BangoScripts/Subsystem/BangoFactSubsystem.h"

#include "BangoFact_int32.generated.h"

USTRUCT()
struct FBangoFact_int32 : public FBangoFactBase
{
	GENERATED_BODY()
	
	BANGO_GENERATED_BODY(int32);
	
	UPROPERTY()
	int32 Value = 0;
};

DEFINE_BANGO_FACT_GETSET(int32);