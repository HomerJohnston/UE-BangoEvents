#pragma once

#include "Bango/Triggers/Targets/Base/BangoTargetBase.h"

#include "BangoTarget_None.generated.h"

UCLASS()
class UBangoTarget_None : public UBangoTargetBase
{
	GENERATED_BODY()
	
	bool IsTarget(AActor* Candidate) override
	{
		// Nobody is valid!
		return false;
	}
};