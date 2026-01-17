#pragma once

#include "BangoScripts/Facts/BangoFactBase.h"
#include "BangoScripts/Subsystem/BangoFactSubsystem.h"

#include "BangoFact_double.generated.h"

USTRUCT()
struct FBangoFact_double : public FBangoFactBase
{
	GENERATED_BODY()
	
	BANGO_GENERATED_BODY(double);
	
	UPROPERTY()
	double Value = 0.0;
};

DEFINE_BANGO_FACT_GETSET(double);