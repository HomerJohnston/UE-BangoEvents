#pragma once
#include "Bango/BangoInstigatorFilter.h"

#include "InstigatorFilter_PlayerPawn.generated.h"

UCLASS(DisplayName="Pawn, Player-Controlled")
class BANGO_API UBangoInstigatorFilter_PlayerPawn : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(UObject* Instigator) override;
};