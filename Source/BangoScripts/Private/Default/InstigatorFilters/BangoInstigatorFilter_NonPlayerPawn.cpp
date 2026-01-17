#include "BangoScripts/Default/InstigatorFilters/BangoInstigatorFilter_NonPlayerPawn.h"

#include "GameFramework/Pawn.h"

bool UBangoInstigatorFilter_NonPlayerPawn::IsValidInstigator_Implementation(UObject* Instigator)
{
	if (APawn* Pawn = Cast<APawn>(Instigator))
	{
		return IsValid(Pawn) && !Pawn->IsLocallyControlled();
	}

	return false;
}
