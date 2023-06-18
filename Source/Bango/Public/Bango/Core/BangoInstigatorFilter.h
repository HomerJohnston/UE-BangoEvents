#pragma once

#include "BangoInstigatorFilter.generated.h"

UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoInstigatorFilter : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsValidInstigator(AActor* EventTriggerActor, AActor* InstigatorActor);
};
