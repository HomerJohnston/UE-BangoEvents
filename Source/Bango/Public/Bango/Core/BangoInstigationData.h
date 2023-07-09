#pragma once

#include "BangoSignal.h"

#include "BangoInstigationData.generated.h"

USTRUCT(BlueprintType)
struct FBangoInstigationData
{
	GENERATED_BODY()
	
	FBangoInstigationData();

	FBangoInstigationData(UObject* InInstigator, double InTime);

	UPROPERTY(BlueprintReadOnly)
	UObject* Instigator;
	
	UPROPERTY(BlueprintReadOnly)
	double Time;

	bool operator ==(const FBangoInstigationData& Other) const;

	bool Equals(UObject* InInstigator) const;
};


USTRUCT(BlueprintType)
struct FBangoInstigationDataCtr
{
	GENERATED_BODY()

	TArray<FBangoInstigationData> Array;
};