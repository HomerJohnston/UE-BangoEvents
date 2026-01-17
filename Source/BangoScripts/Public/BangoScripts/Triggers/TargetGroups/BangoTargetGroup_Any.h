#pragma once

#include "BangoScripts/Triggers/TargetGroups/Base/BangoTargetGroup.h"

#include "BangoTargetGroup_Any.generated.h"

#define LOCTEXT_NAMESPACE "BangoScripts"

UCLASS()
class UBangoTargetGroup_Any : public UBangoTargetGroup
{
	GENERATED_BODY()
	
public:
	UBangoTargetGroup_Any()
	{
#if WITH_EDITOR
		DisplayName = LOCTEXT("BangoTargetGroup_Any_DisplayName", "Any");
		SortOrder = 20;
#endif
	}	
	
	bool IsTarget(AActor* Candidate) override
	{
		// Everybody is valid!
		return true;
	}
};

#undef LOCTEXT_NAMESPACE