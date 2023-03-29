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

UCLASS(DisplayName="Player Character Only")
class UBangoInstigatorFilter_Player : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
public:
	bool IsValidInstigator_Implementation(AActor* EventTriggerActor, AActor* InstigatorActor) override;
};
