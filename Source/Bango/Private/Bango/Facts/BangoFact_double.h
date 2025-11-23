#pragma once

#include "Bango/Facts/BangoFactBase.h"
#include "Bango/Subsystem/BangoFactsSubsystem.h"

#include "BangoFact_double.generated.h"

USTRUCT()
struct FBangoFact_double : public FBangoFactBase
{
	GENERATED_BODY()
	
	BANGO_GENERATED_BODY(double);
	
	UPROPERTY()
	double Value;
};

DEFINE_BANGO_FACT(double);