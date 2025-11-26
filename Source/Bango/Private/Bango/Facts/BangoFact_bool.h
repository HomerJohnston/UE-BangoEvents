#pragma once

#include "Bango/Facts/BangoFactBase.h"
#include "Bango/Subsystem/BangoFactSubsystem.h"

#include "BangoFact_bool.generated.h"

USTRUCT()
struct FBangoFact_bool : public FBangoFactBase
{
	GENERATED_BODY()

	BANGO_GENERATED_BODY(bool);
	
	UPROPERTY()
	bool Value;
};

DEFINE_BANGO_FACT_GETSET(bool);