// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoBlueprintFunctionLibrary.generated.h"

class UBangoScriptObject;
struct FBangoScriptHandle;

UCLASS()
class UBangoBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
    static void AbortScript(UObject* WorldContext, UPARAM(Ref) FBangoScriptHandle& Handle);
};
