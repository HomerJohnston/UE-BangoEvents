#pragma once
#include "Bango/Components/BangoScriptComponent.h"

#include "BangoEditorSubsystem.generated.h"

// Special thanks to https://github.com/ashe23/ProjectCleaner for lots of helper code used in this class.

/**
 * 
 */
class IContentBrowserHideFolderIfEmptyFilter;

UCLASS()
class UBangoEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	bool bDuplicateActorsActive = false;
	
	TArray<TPair<FGuid, TStrongObjectPtr<UBlueprint>>> SoftDeletedScripts;
	
public:
	// Simple solution to force any active FBangoScriptContainerCustomization instances to redraw
	TMulticastDelegate<void()> OnScriptGenerated;
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	FString GetState(UObject* Object) const;

	void OnAssetPostImport(UFactory* Factory, UObject* Object) const;
	void OnPackageDeleted(UPackage* Package) const;
	void OnAssetsAddExtraObjectsToDelete(TArray<UObject*>& Objects) const;
	void OnAssetsPreDelete(const TArray<UObject*>& Objects) const;
	void OnAssetsDeleted(const TArray<UClass*>& Classes);
	void OnDuplicateActorsBegin();
	void OnDuplicateActorsEnd();
	
	void OnLevelActorAdded(AActor* Actor) const;
	void OnLevelActorDeleted(AActor* Actor) const;

	void OnObjectConstructed(UObject* Object) const;
	void OnObjectRenamed(UObject* ObjectL, UObject* Object, FName Name) const;
	void OnAssetLoaded(UObject* Object) const;
	void OnObjectModified(UObject* Object) const;
	
	static TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> Filter;	
	
	void OnScriptComponentCreated(UBangoScriptComponent* BangoScriptComponent) const;
	void OnScriptComponentDestroyed(UBangoScriptComponent* BangoScriptComponent);
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScriptInstance> ScriptClass);
	UBlueprint* RetrieveSoftDeletedScript(FGuid Guid);

	static void DeleteEmptyFolders(const FString& RootPath, bool bShowSlowTask = true);
	
	static void GetFolders(const FString& RootPath, TArray<FString>& OutFolders, bool bSearchRecursive = true);
	static void GetEmptyFolders(const FString& RootPath, TArray<FString>& Folders);

	static bool IsFolderEmpty(const FString& InPath);
	static bool IsFolderEngineGenerated(const FString& InPath);

	static FString PathNormalize(const FString& InPath);
	static FString PathConvertToAbsolute(const FString& InPath);
	static FString PathConvertToRelative(const FString& InPath);
};
