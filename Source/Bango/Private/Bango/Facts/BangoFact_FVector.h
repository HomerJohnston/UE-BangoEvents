#pragma once

#include "Bango/Facts/BangoFactBase.h"
#include "Bango/Subsystem/BangoFactsSubsystem.h"

#include "BangoFact_FVector.generated.h"

USTRUCT()
struct FBangoFact_FVector : public FBangoFactBase
{
	GENERATED_BODY()
	
	BANGO_GENERATED_BODY(FVector);
	
	UPROPERTY()
	FVector Value;
};

DEFINE_BANGO_FACT(FVector);