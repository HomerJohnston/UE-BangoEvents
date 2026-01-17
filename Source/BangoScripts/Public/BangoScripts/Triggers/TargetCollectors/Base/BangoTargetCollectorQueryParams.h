#pragma once

#include "Templates/SubclassOf.h"

#include "BangoTargetCollectorQueryParams.generated.h"

USTRUCT(BlueprintType)
struct BANGOSCRIPTS_API FBangoTargetCollectorQueryParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//EBangoTargetQueryMobileType MobilityTypes = EBangoTargetQueryMobileType::AllDynamicObjects;
	
	bool IsValidMobility(AActor* Actor); 
};