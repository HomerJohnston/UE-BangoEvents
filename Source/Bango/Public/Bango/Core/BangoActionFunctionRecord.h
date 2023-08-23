#pragma once

#include "BangoActionFunctionRecord.generated.h"

USTRUCT(BlueprintType)
struct FBangoActionFunctionRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(EditAnywhere)
	FName FunctionName;

	UPROPERTY(EditAnywhere)
	UFunction* Function;
};