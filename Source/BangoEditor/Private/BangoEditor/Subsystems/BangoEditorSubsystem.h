#pragma once
#include "Bango/Components/BangoScriptComponent.h"

#include "BangoEditorSubsystem.generated.h"

class IContentBrowserHideFolderIfEmptyFilter;

UCLASS()
class UBangoEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	bool bDuplicateActorsActive = false;
	
	TArray<TPair<FGuid, TStrongObjectPtr<UBlueprint>>> SoftDeletedScripts;
	
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
};
