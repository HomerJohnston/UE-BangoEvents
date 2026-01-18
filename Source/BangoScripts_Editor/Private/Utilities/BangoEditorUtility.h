#pragma once
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"

struct FBangoScriptContainer;
class UBangoScriptBlueprint;
class UBangoScriptComponent;
class UBangoScript;
class IPropertyHandle;

namespace Bango::Editor
{
	// returns "__BangoScript__".
	FString GetGameScriptRootFolder();

	// returns '~'
	const TCHAR* GetLevelScriptNamePrefix();
	
	// Gets the full computer drive path to the script folder.
	FString GetAbsoluteScriptRootFolder();
	
	// Normal starting point for making a new package.
	UPackage* MakeLevelScriptPackage(UObject* Outer, FGuid Guid);

	// Actual function that makes the .uasset file containing a level script package.
	UPackage* MakeLevelScriptPackage_Internal(AActor* Actor, UObject* Outer, FGuid Guid);
	
	// Actual function that makes a new UBangoScriptBlueprint level script blueprint.
	UBangoScriptBlueprint* MakeLevelScript(UPackage* InPackage, const FString& InName, const FGuid& InGuid);

	// Actual function that duplicates a UBangoScriptBlueprint level script blueprint.
	UBangoScriptBlueprint* DuplicateLevelScript(UBangoScriptBlueprint* SourceBlueprint, UPackage* NewScriptPackage, const FString& InName, const FGuid& NewGuid, AActor* NewOwnerActor);
	
	// I can't find any existing PUBLIC code in this retarded engine to do this, so now I have to copy code from AssetViewUtils.
	bool DeleteEmptyFolderFromDisk(const FString& InPathToDelete);
	
	// Changes "BlueprintName" to "~BlueprintName", this is to make it hidden in the content browser without engine modifications.
	FString GetLocalScriptName(FString InName);
}
