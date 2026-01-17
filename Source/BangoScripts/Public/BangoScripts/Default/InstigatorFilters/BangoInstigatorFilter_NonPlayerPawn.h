#pragma once
#include "BangoScripts/BangoInstigatorFilter.h"

#include "BangoInstigatorFilter_NonPlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Not Player-Controlled")
class BANGOSCRIPTS_API UBangoInstigatorFilter_NonPlayerPawn : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(UObject* Instigator) override;
};