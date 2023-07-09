#pragma once

#include "BangoInterfaces.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBangoBangEventActionInterface : public UInterface
{
	GENERATED_BODY()
};

class BANGO_API IBangoBangEventActionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnStart();
};




UINTERFACE(MinimalAPI, Blueprintable)
class UBangoToggleEventActionInterface : public UInterface
{
	GENERATED_BODY()
};

class BANGO_API IBangoToggleEventActionInterface
{
	GENERATED_BODY()

public:
	/** Performs action logic. You do not need to call Super implementation when overriding. */
	UFUNCTION(BlueprintNativeEvent)
	void OnStart();

	/** Performs action logic. You do not need to call Super implementation when overriding. */
	UFUNCTION(BlueprintNativeEvent)
	void OnStop();
};