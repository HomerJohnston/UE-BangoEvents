#pragma once

#include "Bango/Triggers/TargetGroups/Base/BangoTargetGroup.h"

#include "BangoTargetGroup_None.generated.h"

#define LOCTEXT_NAMESPACE "Bango"

UCLASS()
class UBangoTargetGroup_None : public UBangoTargetGroup
{
	GENERATED_BODY()
	
public:
	UBangoTargetGroup_None()
	{
		DisplayName = LOCTEXT("BangoTargetGroup_None_DisplayName", "None");
		
		SortOrder = 10;
	}	
	
	bool IsTarget(AActor* Candidate) override
	{
		// Nobody is valid!
		return false;
	}
};

#undef LOCTEXT_NAMESPACE