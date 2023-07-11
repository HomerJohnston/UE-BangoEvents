#include "Bango/DefaultImpl/InstigatorFilters/BangoInstigatorFilter_ByClass.h"

bool UBangoInstigatorFilter_ByClass::IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor)
{
	return InstigatorActor->IsA(RequiredType);
}
