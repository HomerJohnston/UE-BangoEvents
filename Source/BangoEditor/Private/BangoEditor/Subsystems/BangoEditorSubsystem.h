#pragma once

#include "Bango/Components/BangoScriptComponent.h"

#include "BangoEditorSubsystem.generated.h"

class UObject;

namespace Bango
{
	struct FScriptContainerKey
	{
		FScriptContainerKey(TWeakObjectPtr<UObject> InScriptOuter, FBangoScriptContainer* InScriptContainer)
			: ScriptOuter(InScriptOuter)
			, ScriptContainer(InScriptContainer)
		{
			Script = ScriptContainer->GetScriptClass();
			
			UObject* Outer = InScriptOuter.Get(true);
			
			Outer->Modify();
			
			InScriptContainer->Unset();
		}
		
		TWeakObjectPtr<UObject> ScriptOuter;
		FBangoScriptContainer* ScriptContainer;
		
		TSoftClassPtr<UBangoScript> Script;
		
		bool operator==(const FScriptContainerKey& Other) const
		{
			return ScriptOuter == Other.ScriptOuter && ScriptContainer == Other.ScriptContainer;
		}
		
		friend int32 GetTypeHash(const FScriptContainerKey& ScriptContainerKey)
		{
			return HashCombine(ScriptContainerKey.ScriptOuter.GetWeakPtrTypeHash(), GetTypeHash(ScriptContainerKey.ScriptContainer));
		}
	};
}

using namespace Bango;


// Special thanks to https://github.com/ashe23/ProjectCleaner for lots of helper code used in this class.

/*
struct FBangoCreatedScriptContainer
{
	// The object that is responsible for creating the script container. 
	// We'll check if this object stays valid after 1 frame, and if so, populate the script container with a new  script.  
	TWeakObjectPtr<UObject> Object;
	
	// The script container that will be populated.
	FBangoScriptContainer* ScriptContainer;
	
	bool operator==(const FBangoCreatedScriptContainer& Other) const
	{
		return Object == Other.Object && ScriptContainer == Other.ScriptContainer;
	}
};

struct FBangoDestroyedScriptContainer
{
	// The destroyed script class. 
	TSubclassOf<UBangoScript> ScriptClass;
	
	bool operator==(const FBangoDestroyedScriptContainer& Other) const
	{
		return Object == Other.Object && ScriptClass == Other.ScriptClass;
	}
};

uint32 GetTypeHash(const FBangoCreatedScriptContainer& ScriptContainerRef)
{
	return HashCombine(GetTypeHash(ScriptContainerRef.Object), GetTypeHash(ScriptContainerRef.ScriptContainer));
}

uint32 GetTypeHash(const FBangoDestroyedScriptContainer& ScriptContainerRef)
{
	return HashCombine(GetTypeHash(ScriptContainerRef.Object), GetTypeHash(ScriptContainerRef.ScriptClass));
}
*/

/**
 * 
 */
class IContentBrowserHideFolderIfEmptyFilter;

UCLASS()
class UBangoLevelScriptsEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	static UBangoLevelScriptsEditorSubsystem* Get();
	
protected:
	bool bDuplicateActorsActive = false;
	bool bDeleteActorsActive = false;
	
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
	void OnDeleteActorsBegin();
	void OnDeleteActorsEnd();
	
	void OnLevelActorAdded(AActor* Actor) const;
	void OnLevelActorDeleted(AActor* Actor) const;

	void OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap);
	void PreSaveWorldWithContext(UWorld* World, FObjectPreSaveContext ObjectPreSaveContext) const;
	
	void OnObjectConstructed(UObject* Object) const;
	void OnObjectRenamed(UObject* ObjectL, UObject* RenamedObjectOuter, FName OldName) const;
	void OnAssetLoaded(UObject* Object) const;
	void OnObjectModified(UObject* Object) const;
	
	static TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> Filter;	
	
	void OnLevelScriptContainerCreated(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString BlueprintName = "");

	void OnLevelScriptContainerDestroyed(UObject* Outer, FBangoScriptContainer* ScriptContainer);
	// bool IsExistingScriptContainerValid(UObject* Outer, FBangoScriptContainer* ScriptContainer);
	void OnLevelScriptContainerUnregisteredDuringTransaction(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptClass);

	void OnLevelScriptContainerDuplicated(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString BlueprintName = "");
	
	void OnRequestNewID(AActor* Actor) const;
	
	void SoftDeleteLevelScriptPackage(TSoftClassPtr<UBangoScript> ScriptClass);
	static UBangoScriptBlueprint* RetrieveDeletedLevelScript(FGuid Guid);
	
private:
	void EnqueueCreatedScriptComponent(UObject* Owner, FBangoScriptContainer* ScriptContainer);
	
	void EnqueueDestroyedScriptComponent(UObject* Owner, FBangoScriptContainer* ScriptContainer);
	
private:
enum class EScriptState : uint8
{
	Created,
	Destroyed,
};
	
	// When a component is created in the editor, it gets queued here. On the next tick, ProcessScriptRequestQueues will flush these. If a script is created and then destroyed, it will be added and removed to this queue. 
	TMap<FScriptContainerKey, EScriptState> QueuedModifiedScriptComponents;
	
	FTimerHandle ProcessScriptRequestQueuesHandle;
	
	void QueueProcessScriptRequestQueues();
	
	void ProcessScriptRequestQueues();
	
	void ProcessCreatedScriptRequest(TWeakObjectPtr<UObject> Owner, FBangoScriptContainer* ScriptContainer);
	
	void ProcessDestroyedScriptRequest(TWeakObjectPtr<UObject> Owner, TSoftClassPtr<UBangoScript> ScriptClass);
	
	void CreateScript(UObject* Outer, FBangoScriptContainer* ScriptContainer);
	
	void DestroyScript(TSoftClassPtr<UBangoScript> ScriptClass);
};
