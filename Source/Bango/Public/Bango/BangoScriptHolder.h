#pragma once

#include "BangoScriptHolder.generated.h"

class UBangoScriptObject;

USTRUCT()
struct FBangoScriptHolder
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlueprint> ScriptBlueprint;
#endif
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBangoScriptObject> ScriptClass;
};
