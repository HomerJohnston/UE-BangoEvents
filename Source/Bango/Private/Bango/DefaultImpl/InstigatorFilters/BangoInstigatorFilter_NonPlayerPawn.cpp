#include "Bango/DefaultImpl/InstigatorFilters/BangoInstigatorFilter_NonPlayerPawn.h"

bool UBangoInstigatorFilter_NonPlayerPawn::IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor)
{
	if (APawn* Pawn = Cast<APawn>(InstigatorActor))
	{
		return IsValid(Pawn) && !Pawn->IsLocallyControlled();
	}

	return false;
}
