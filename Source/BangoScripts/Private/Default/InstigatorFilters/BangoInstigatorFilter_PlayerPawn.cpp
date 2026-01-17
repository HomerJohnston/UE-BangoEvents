#include "BangoScripts/Default/InstigatorFilters/BangoInstigatorFilter_PlayerPawn.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

bool UBangoInstigatorFilter_PlayerPawn::IsValidInstigator_Implementation(UObject* Instigator)
{
	UObject* Owner = Instigator;

	// If the instigator is a UObject or a UActorComponent, try to follow an outer chain to an actor and make that the real instigator
	// This is still a fragile implementation; it does not for example allow for attached actors to trigger correctly
	while (IsValid(Owner) && !Owner->IsA(AActor::StaticClass()))
	{
		Owner = Instigator->GetOuter();

		if (!Owner)
		{
			break;
		}
	};

	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		return IsValid(Pawn) && Pawn->IsLocallyControlled();
	}
	
	return false;
}
