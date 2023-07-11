// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoInstigatorFilter.h"
#include "BangoInstigatorFilter_ByClass.generated.h"

UCLASS(DisplayName="Specific Class")
class BANGO_API UBangoInstigatorFilter_ByClass : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	UPROPERTY(EditAnywhere, Category="Settings")
	TSubclassOf<UObject> RequiredType;
	
public:
	bool IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor) override;
};
