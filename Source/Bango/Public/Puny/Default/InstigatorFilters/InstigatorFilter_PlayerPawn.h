#pragma once
#include "Puny/InstigatorFilter.h"

#include "InstigatorFilter_PlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Player-Controlled")
class BANGO_API UPunyInstigatorFilter_PlayerPawn : public UPunyInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(UObject* Instigator) override;
};