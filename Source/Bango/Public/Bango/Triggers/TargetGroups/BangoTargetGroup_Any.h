#pragma once

#include "Bango/Triggers/TargetGroups/Base/BangoTargetGroup.h"

#include "BangoTargetGroup_Any.generated.h"

#define LOCTEXT_NAMESPACE "Bango"

UCLASS()
class UBangoTargetGroup_Any : public UBangoTargetGroup
{
	GENERATED_BODY()
	
public:
	UBangoTargetGroup_Any()
	{
		DisplayName = LOCTEXT("BangoTargetGroup_Any_DisplayName", "Any");
		
		SortOrder = 20;
	}	
	
	bool IsTarget(AActor* Candidate) override
	{
		// Everybody is valid!
		return true;
	}
};

#undef LOCTEXT_NAMESPACE