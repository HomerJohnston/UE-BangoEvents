#include "Bango/Core/BangoInstigatorFilter.h"

#include "GameFramework/Character.h"

bool UBangoInstigatorFilter::IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor)
{
	return true;
}

bool UBangoInstigatorFilter_Player::IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor)
{
	if (ACharacter* Character = Cast<ACharacter>(InstigatorActor))
	{
		return IsValid(Character) && Character->IsLocallyControlled();
	}

	return false;
}
