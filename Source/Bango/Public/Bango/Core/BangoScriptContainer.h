#pragma once

#include "Bango/Core/BangoScriptBlueprint.h"

#include "BangoScriptContainer.generated.h"

class UBangoScript;
class UBangoScriptBlueprint;

/**
 * This struct is used to hold a script. It is used in ABangoTrigger and in UBangoScriptComponent.
 */
USTRUCT()
struct FBangoScriptContainer
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
	// This will be kept in sync with the UBangoScriptObject's ScriptGuid and is used for undo/redo purposes and other sync
	UPROPERTY(VisibleAnywhere)
	FGuid Guid;
	
	//UPROPERTY(VisibleAnywhere)
	//TObjectPtr<UBangoScriptBlueprint> ScriptBlueprint;
#endif
	
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UBangoScript> ScriptClass;
	
#if WITH_EDITOR
	void PrepareForDestroy();
#endif
};
