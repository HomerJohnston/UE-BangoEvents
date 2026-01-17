#pragma once

#include "BangoInstigatorFilter.generated.h"

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGOSCRIPTS_API UBangoInstigatorFilter : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsValidInstigator(UObject* Instigator);
};
