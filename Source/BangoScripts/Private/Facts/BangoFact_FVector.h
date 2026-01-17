#pragma once

#include "BangoScripts/Facts/BangoFactBase.h"
#include "BangoScripts/Subsystem/BangoFactSubsystem.h"

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