// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoInstigatorFilter.h"
#include "BangoInstigatorFilter_NonPlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Not Player-Controlled")
class UBangoInstigatorFilter_NonPlayerPawn : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor) override;
};
