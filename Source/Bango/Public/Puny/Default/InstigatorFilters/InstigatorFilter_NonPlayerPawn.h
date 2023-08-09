#pragma once
#include "Puny/Core/InstigatorFilter.h"

#include "InstigatorFilter_NonPlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Not Player-Controlled")
class BANGO_API UPunyInstigatorFilter_NonPlayerPawn : public UPunyInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(UObject* Instigator) override;
};