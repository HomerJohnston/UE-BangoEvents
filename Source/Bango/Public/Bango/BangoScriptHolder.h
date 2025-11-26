#pragma once

#include "BangoScriptHolder.generated.h"

class UBangoScriptObject;

USTRUCT()
struct FBangoScriptHolder
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSubclassOf<UBangoScriptObject> ScriptClass;
};
