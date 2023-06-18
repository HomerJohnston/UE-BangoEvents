#pragma once

#include "Bango/Core/BangoInstigatorFilter.h"
#include "BangoInstigatorFilter_PlayerOnly.generated.h"

UCLASS(DisplayName="Player Character Only")
class UBangoInstigatorFilter_Player : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor) override;
};
