#pragma once

class UBangoScriptComponent;
class UBangoScriptInstance;

namespace Bango::Editor
{
	static FString ScriptRootFolder = TEXT("__BangoScripts__");
	
	FString GetScriptRootContentFolder();
	
	AActor* GetActorOwner(TSharedPtr<IPropertyHandle> Property);
	
	UPackage* MakeScriptPackage(UBangoScriptComponent* Component, UObject* Outer, FString& NewBPName);

	UPackage* MakeScriptPackage(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& NewBPName);
	
	UPackage* MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& NewBPName);
	
	UBlueprint* MakeScriptAsset(UPackage* InPackage, FString Name, FGuid Guid);
	
	bool SaveScriptPackage(UPackage* ScriptPackage, UBlueprint* ScriptBlueprint);
	
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScriptInstance> ScriptClass);
	
	// I can't find any existing PUBLIC code in this retarded engine to do this, so now I have to copy code from AssetViewUtils
	bool DeleteEmptyFolderFromDisk(const FString& InPathToDelete);
}
