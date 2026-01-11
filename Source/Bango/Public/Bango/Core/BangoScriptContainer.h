#pragma once

#include "Bango/Core/BangoScriptBlueprint.h"
#include "Engine/StreamableManager.h"

#include "BangoScriptContainer.generated.h"

class UBangoScript;
class UBangoScriptBlueprint;

/**
 * This struct is used to hold a script. It is used in ABangoTrigger and in UBangoScriptComponent.
 */
USTRUCT()
struct BANGO_API FBangoScriptContainer
{
	GENERATED_BODY()
	
	friend class FBangoScriptContainerCustomization;
	
public:
	//FBangoScriptHandle Run(UObject* Runner, bool bImmediate);
	
	const TSoftClassPtr<UBangoScript>& GetScriptClass() const { return ScriptClass; }

private:
	UPROPERTY(VisibleAnywhere)
	TSoftClassPtr<UBangoScript> ScriptClass;
	
	TSharedPtr<FStreamableHandle> ScriptClassHandle;

	// TODO I can probably delete this but keeping it in until the plugin is stable
	// This is only necessary if packaging scripts into .umap files... and packaging into .umap doesn't work with World Partition because world partition package splitting breaks the script packages 
	/** In standalone this returns the ScriptClass. In PIE this strips any "PIE" info out of the class path. */
	FSoftObjectPath GetSanitizedScriptClass() const;
	
#if WITH_EDITORONLY_DATA
private:
	// This will be kept in sync with the UBangoScriptObject's ScriptGuid and is used for undo/redo purposes and other sync
	UPROPERTY(VisibleAnywhere)
	FGuid Guid;
	
public:
	const FGuid& GetGuid() const { return Guid; }
	
	void SetGuid(const FGuid& InGuid);
#endif
	
public:
#if WITH_EDITOR
	void Unset();
	
	void Reset();
	
	void SetScriptClass(TSubclassOf<UObject> NewScriptClass)
	{
		ScriptClass = NewScriptClass;
	}
	
	void TryRestoreScriptFromTransientPackage();

#endif
};
