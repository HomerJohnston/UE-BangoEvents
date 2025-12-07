#pragma once

#include "BangoScriptHolder.generated.h"

class UBangoScriptInstance;

/**
 * This struct is used to hold a script. It is used in ABangoTrigger and in UBangoScriptComponent.
 */
USTRUCT()
struct FBangoScriptContainer
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
	// This will be kept in sync with the UBangoScriptObject's ScriptGuid and is used for undo/redo purposes and other sync
	UPROPERTY(EditAnywhere)
	FGuid Guid;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlueprint> ScriptBlueprint;
#endif
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBangoScriptInstance> ScriptClass;
	
#if WITH_EDITOR
	void PrepareForDestroy();
#endif
};
