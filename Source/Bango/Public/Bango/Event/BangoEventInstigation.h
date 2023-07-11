// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoEventInstigation.generated.h"

USTRUCT(BlueprintType)
struct FBangoEventInstigation
{
	GENERATED_BODY()
	
	FBangoEventInstigation();

	FBangoEventInstigation(UObject* InInstigator, double InTime);

	UPROPERTY(BlueprintReadOnly)
	UObject* Instigator;
	
	UPROPERTY(BlueprintReadOnly)
	double Time;

	bool operator ==(const FBangoEventInstigation& Other) const;

	bool Equals(UObject* InInstigator) const;
};

USTRUCT(BlueprintType)
struct FBangoEventInstigationArray
{
	GENERATED_BODY()

	TArray<FBangoEventInstigation> Array;
};