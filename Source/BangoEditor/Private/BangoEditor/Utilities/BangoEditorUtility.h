#pragma once
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dialogs/Dialogs.h"
#include "Kismet2/KismetEditorUtilities.h"

struct FBangoScriptContainer;
class UBangoScriptBlueprint;
class UBangoScriptComponent;
class UBangoScript;

namespace Bango::Editor
{
	static FString ScriptRootFolder = TEXT("__BangoScripts__");
		
	static TMulticastDelegate<void()> OnScriptGenerated;
	
	FString GetGameScriptRootFolder();
	
	FString GetAbsoluteScriptRootFolder();
	
	AActor* GetActorOwner(TSharedPtr<IPropertyHandle> Property);
	
	// Normal starting point for making a new package
	UPackage* MakeLevelScriptPackage(UObject* Outer, FString& InOutBPName, FGuid Guid);

	// Intended to be used from a details customization; currently unused, left in case I want it later
	UPackage* MakeLevelScriptPackage(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& InOutBPName, FGuid Guid);
	
	// Actual function that makes the .uasset file containing a level script package
	UPackage* MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& InOutBPName, FGuid Guid);
	
	// Actual function that makes the UBangoScriptBlueprint for a level script
	UBangoScriptBlueprint* MakeScriptAsset(UPackage* InPackage, const FString& Name, FGuid Guid);
	
	bool SaveScriptPackage(UPackage* ScriptPackage, UBlueprint* ScriptBlueprint);
	
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScript> ScriptClass);
	
	// I can't find any existing PUBLIC code in this retarded engine to do this, so now I have to copy code from AssetViewUtils
	bool DeleteEmptyFolderFromDisk(const FString& InPathToDelete);
	
	void NewScriptRequested(UObject* Outer, FBangoScriptContainer* ScriptContainer, FGuid Guid = FGuid());
	
}
