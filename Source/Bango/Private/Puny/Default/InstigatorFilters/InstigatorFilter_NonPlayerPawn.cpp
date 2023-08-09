﻿#include "Puny/Default/InstigatorFilters/InstigatorFilter_NonPlayerPawn.h"

bool UPunyInstigatorFilter_NonPlayerPawn::IsValidInstigator_Implementation(UObject* Instigator)
{
	if (APawn* Pawn = Cast<APawn>(Instigator))
	{
		return IsValid(Pawn) && !Pawn->IsLocallyControlled();
	}

	return false;
}
