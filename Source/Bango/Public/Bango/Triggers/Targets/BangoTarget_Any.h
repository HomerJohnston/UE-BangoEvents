#pragma once

#include "Bango/Triggers/Targets/Base/BangoTargetBase.h"

#include "BangoTarget_Any.generated.h"

UCLASS()
class UBangoTarget_Any : public UBangoTargetBase
{
	GENERATED_BODY()
	
	bool IsTarget(AActor* Candidate) override
	{
		// Everybody is valid!
		return true;
	}
};