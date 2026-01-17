#pragma once

#include "BangoAssetUserData.generated.h"

UCLASS()
class BANGOSCRIPTS_API UBangoUserAssetData : public UAssetUserData
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UObject* BlueprintRef; 
	
	UPROPERTY()
	float Floatvalue;
	
	UPROPERTY()
	bool bBoolValue;
};