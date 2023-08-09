#include "Puny/Default/InstigatorFilters/InstigatorFilter_PlayerPawn.h"

bool UPunyInstigatorFilter_PlayerPawn::IsValidInstigator_Implementation(UObject* Instigator)
{
	if (APawn* Pawn = Cast<APawn>(Instigator))
	{
		return IsValid(Pawn) && Pawn->IsLocallyControlled();
	}

	return false;
}
