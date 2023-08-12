#pragma once

#include "InstigatorFilter.generated.h"

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoInstigatorFilter : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsValidInstigator(UObject* Instigator);
};
