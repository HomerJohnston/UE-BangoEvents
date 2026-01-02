#pragma once

#include "Bango/Facts/BangoFactBase.h"
#include "Bango/Subsystem/BangoFactSubsystem.h"

#include "BangoFact_FVector.generated.h"

USTRUCT()
struct FBangoFact_FVector : public FBangoFactBase
{
	GENERATED_BODY()
	
	BANGO_GENERATED_BODY(FVector);
	
	UPROPERTY()
	FVector Value = FVector::ZeroVector;
};

DEFINE_BANGO_FACT_GETSET(FVector);