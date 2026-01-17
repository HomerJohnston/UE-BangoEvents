#pragma once

#include "BangoScripts/Facts/BangoFactBase.h"
#include "BangoScripts/Subsystem/BangoFactSubsystem.h"

#include "BangoFact_bool.generated.h"

USTRUCT()
struct FBangoFact_bool : public FBangoFactBase
{
	GENERATED_BODY()

	BANGO_GENERATED_BODY(bool);
	
	UPROPERTY()
	bool Value = false;
};

DEFINE_BANGO_FACT_GETSET(bool);