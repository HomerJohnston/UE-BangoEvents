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
	/** A brief description of the blueprint. This can be displayed in the level editor viewport. */
	UPROPERTY(EditInstanceOnly)
	FString Description = "";
	
	UPROPERTY(VisibleAnywhere)
	TSoftClassPtr<UBangoScript> ScriptClass = nullptr;
	
	// Used for async loading
	TSharedPtr<FStreamableHandle> ScriptClassHandle = nullptr;

#if WITH_EDITORONLY_DATA
	// Used during construction of the ScriptClass only
	FString RequestedName = "";
	
	bool bIsDuplicate = false;
#endif
	
	// TODO I can probably delete this but keeping it in until the plugin is stable
	// This is only necessary if packaging scripts into .umap files... and packaging into .umap doesn't work with World Partition because world partition package splitting breaks the script packages 
	/** In standalone this returns the ScriptClass. In PIE this strips any "PIE" info out of the class path. */
	FSoftObjectPath GetSanitizedScriptClass() const;
	
#if WITH_EDITORONLY_DATA
private:
	// This will be kept in sync with the UBangoScriptObject's ScriptGuid and is used for undo/redo purposes and other sync
	UPROPERTY()
	FGuid Guid;
	
public:
	const FGuid& GetGuid() const { return Guid; }
	
	void SetGuid(const FGuid& InGuid);
#endif
	
public:
#if WITH_EDITOR
	void Unset();
	
	void SetScriptClass(TSubclassOf<UObject> NewScriptClass)
	{
		ScriptClass = NewScriptClass;
	}
	
	void SetRequestedName(const FString& InName)
	{
		RequestedName = InName;
	}
	
	const FString& GetRequestedName() const
	{
		return RequestedName;
	}
	
	void SetIsDuplicate()
	{
		bIsDuplicate = true;
	}
	
	bool ConsumeDuplicate()
	{
		bool bWasDuplicate = bIsDuplicate;
		bIsDuplicate = false;
		return bWasDuplicate;
	}
	
	const FString& GetDescription() const
	{
		return Description;
	}
#endif
};
