#pragma once
#include "Bango/BangoInstigatorFilter.h"

#include "BangoInstigatorFilter_NonPlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Not Player-Controlled")
class BANGO_API UBangoInstigatorFilter_NonPlayerPawn : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(UObject* Instigator) override;
};