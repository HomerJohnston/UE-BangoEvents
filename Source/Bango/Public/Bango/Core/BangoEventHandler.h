#pragma once

#include "BangoEventHandler.generated.h"

UCLASS(Abstract)
class BANGO_API UBangoEventHandler : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class BANGO_API UBangoEventHandler_Bang : public UBangoEventHandler
{
	GENERATED_BODY()
};