#include "Bango/Default/InstigatorFilters/BangoInstigatorFilter_NonPlayerPawn.h"

bool UBangoInstigatorFilter_NonPlayerPawn::IsValidInstigator_Implementation(UObject* Instigator)
{
	if (APawn* Pawn = Cast<APawn>(Instigator))
	{
		return IsValid(Pawn) && !Pawn->IsLocallyControlled();
	}

	return false;
}
