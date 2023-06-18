#include "Bango/DefaultImpl/InstigatorFilters/BangoInstigatorFilter_PlayerOnly.h"

#include "GameFramework/Character.h"

bool UBangoInstigatorFilter_Player::IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor)
{
	if (ACharacter* Character = Cast<ACharacter>(InstigatorActor))
	{
		return IsValid(Character) && Character->IsLocallyControlled();
	}

	return false;
}
