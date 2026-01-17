#pragma once

#include "BangoScripts/Triggers/TargetGroups/Base/BangoTargetGroup.h"

#include "BangoTargetGroup_None.generated.h"

#define LOCTEXT_NAMESPACE "BangoScripts"

UCLASS()
class UBangoTargetGroup_None : public UBangoTargetGroup
{
	GENERATED_BODY()
	
public:
	UBangoTargetGroup_None()
	{
#if WITH_EDITOR
		DisplayName = LOCTEXT("BangoTargetGroup_None_DisplayName", "None");
		SortOrder = 10;
#endif
	}	
	
	bool IsTarget(AActor* Candidate) override
	{
		// Nobody is valid!
		return false;
	}
};

#undef LOCTEXT_NAMESPACE