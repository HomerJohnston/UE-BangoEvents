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

public:
	static UBangoEditorSubsystem* Get();
	
protected:
	bool bDuplicateActorsActive = false;
	
	//TArray<TPair<FGuid, TStrongObjectPtr<UBangoScriptBlueprint>>> DeletedScripts;
	
public:
	// Simple solution to force any active FBangoScriptContainerCustomization instances to redraw
	TMulticastDelegate<void()> OnScriptGenerated;
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	FString GetState(UObject* Object) const;

	void OnObjectPreSave(UObject* Object, FObjectPreSaveContext ObjectPreSaveContext) const;
	void OnObjectTransacted(UObject* Object, const class FTransactionObjectEvent& TransactionEvent);

	void OnAssetPostImport(UFactory* Factory, UObject* Object) const;
	void OnPackageDeleted(UPackage* Package) const;
	void OnAssetsAddExtraObjectsToDelete(TArray<UObject*>& Objects) const;
	void OnAssetsPreDelete(const TArray<UObject*>& Objects) const;
	void OnAssetsDeleted(const TArray<UClass*>& Classes);
	void OnDuplicateActorsBegin();
	void OnDuplicateActorsEnd();
	
	void OnLevelActorAdded(AActor* Actor) const;
	void OnLevelActorDeleted(AActor* Actor) const;

	void OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap);
	void PreSaveWorldWithContext(UWorld* World, FObjectPreSaveContext ObjectPreSaveContext) const;
	
	void OnObjectConstructed(UObject* Object) const;
	void OnObjectRenamed(UObject* ObjectL, UObject* RenamedObjectOuter, FName OldName) const;
	void OnAssetLoaded(UObject* Object) const;
	void OnObjectModified(UObject* Object) const;
	
	static TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> Filter;	
	
	void OnScriptContainerCreated(UObject* Outer, FBangoScriptContainer* ScriptContainer);
	void OnScriptContainerDestroyed(UObject* Outer, FBangoScriptContainer* ScriptContainer);
	void OnScriptContainerDuplicated(UObject* Outer, FBangoScriptContainer* ScriptContainer);
	
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScript> ScriptClass);
	static UBangoScriptBlueprint* RetrieveDeletedScript(FGuid Guid);
	
};
