#pragma once

#include "Base/BangoTargetGroup.h"

#include "BangoTargetGroup_ActorType.generated.h"

#define LOCTEXT_NAMESPACE "Bango"

UCLASS()
class UBangoTargetGroup_ActorType : public UBangoTargetGroup
{
	GENERATED_BODY()
	
public:
	UBangoTargetGroup_ActorType()
	{
		DisplayName = LOCTEXT("BangoTargetGroup_ActorTyoe_DisplayName", "Actor Type");
		SortOrder = 40;
	}
};

#undef LOCTEXT_NAMESPACE