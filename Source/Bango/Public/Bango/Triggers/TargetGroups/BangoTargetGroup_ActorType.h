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
#if WITH_EDITOR
		DisplayName = LOCTEXT("BangoTargetGroup_ActorTyoe_DisplayName", "Actor Type");
		SortOrder = 40;
#endif
	}
};

#undef LOCTEXT_NAMESPACE