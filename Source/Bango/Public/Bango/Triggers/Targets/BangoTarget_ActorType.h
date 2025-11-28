#pragma once

#include "Bango/Triggers/Targets/Base/BangoTargetBase.h"

#include "BangoTarget_ActorType.generated.h"

UCLASS()
class UBangoTarget_ActorType : public UBangoTargetBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> Type;
};