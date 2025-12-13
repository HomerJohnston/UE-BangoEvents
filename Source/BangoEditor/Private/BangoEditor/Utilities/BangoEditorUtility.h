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
	
	UPackage* MakePackageForScript(UObject* Outer, FString& NewBPName);

	UPackage* MakePackageForScript(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& NewBPName);
	
	UPackage* MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& NewBPName);
	
	UBangoScriptBlueprint* MakeScriptAsset(UPackage* InPackage, FString Name, FGuid Guid);
	
	bool SaveScriptPackage(UPackage* ScriptPackage, UBlueprint* ScriptBlueprint);
	
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScript> ScriptClass);
	
	// I can't find any existing PUBLIC code in this retarded engine to do this, so now I have to copy code from AssetViewUtils
	bool DeleteEmptyFolderFromDisk(const FString& InPathToDelete);
	
	void NewScriptRequested(UObject* Outer, FBangoScriptContainer* ScriptContainer, FGuid Guid = FGuid());
	
}
