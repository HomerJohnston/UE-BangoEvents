// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoInstigatorFilter.h"
#include "BangoInstigatorFilter_PlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Player-Controlled")
class UBangoInstigatorFilter_PlayerPawn : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor) override;
};
