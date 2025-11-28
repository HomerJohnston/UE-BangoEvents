#pragma once

#include "BangoTargetLocatorQueryParams.generated.h"

UENUM(BlueprintType)
enum class EBangoTargetQueryMobileType : uint8
{
	AllDynamicObjects,
	AllStaticObjects,
	AllObjects,
};

USTRUCT(BlueprintType)
struct BANGO_API FBangoTargetLocatorQueryParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBangoTargetQueryMobileType QueryType = EBangoTargetQueryMobileType::AllDynamicObjects;
	
	bool IsValidMobility(AActor* Actor); 
};