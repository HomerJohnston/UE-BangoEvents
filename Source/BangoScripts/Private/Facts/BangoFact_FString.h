#pragma once

#include "BangoScripts/Facts/BangoFactBase.h"
#include "BangoScripts/Subsystem/BangoFactSubsystem.h"

#include "BangoFact_FString.generated.h"

USTRUCT()
struct FBangoFact_FString : public FBangoFactBase
{
	GENERATED_BODY()
	
	BANGO_GENERATED_BODY(FString);
	
	UPROPERTY()
	FString Value;
};

DEFINE_BANGO_FACT_GETSET(FString);
