#include "Bango/Default/InstigatorFilters/InstigatorFilter_PlayerPawn.h"

bool UBangoInstigatorFilter_PlayerPawn::IsValidInstigator_Implementation(UObject* Instigator)
{
	if (APawn* Pawn = Cast<APawn>(Instigator))
	{
		return IsValid(Pawn) && Pawn->IsLocallyControlled();
	}

	return false;
}
