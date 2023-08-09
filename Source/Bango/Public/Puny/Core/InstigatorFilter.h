#pragma once

#include "InstigatorFilter.generated.h"

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UPunyInstigatorFilter : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsValidInstigator(UObject* Instigator);
};
