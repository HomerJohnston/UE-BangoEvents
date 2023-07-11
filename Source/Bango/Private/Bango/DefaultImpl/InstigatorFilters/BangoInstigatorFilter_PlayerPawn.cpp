#include "Bango/DefaultImpl/InstigatorFilters/BangoInstigatorFilter_PlayerPawn.h"

#include "GameFramework/Pawn.h"

bool UBangoInstigatorFilter_PlayerPawn::IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor)
{
	if (APawn* Pawn = Cast<APawn>(InstigatorActor))
	{
		return IsValid(Pawn) && Pawn->IsLocallyControlled();
	}

	return false;
}
