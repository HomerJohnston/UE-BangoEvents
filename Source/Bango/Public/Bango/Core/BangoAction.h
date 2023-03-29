#pragma once

#include "BangoAction.generated.h"

UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Run(AActor* EventActor, UObject* Instigator);
};
