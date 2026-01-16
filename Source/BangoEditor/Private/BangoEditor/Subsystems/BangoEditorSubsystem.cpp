#include "BangoEditorSubsystem.h"

#include "ContentBrowserDataSubsystem.h"
#include "IContentBrowserDataModule.h"
#include "ISourceControlModule.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScript.h"
#include "Bango/Core/BangoScriptContainer.h"
#include "BangoEditorTooling/BangoEditorDelegates.h"
#include "BangoEditorTooling/BangoScriptHelperSubsystem.h"
#include "BangoEditorTooling/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/DevTesting/BangoDummyObject.h"
#include "BangoEditor/Menus/BangoEditorMenus.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "BangoEditor/Utilities/BangoFolderUtility.h"
#include "BangoEditor/Private/BangoEditor/Unsorted/BangoHideScriptFolderFilter.h"
#include "BangoEditorTooling/BangoEditorLog.h"
#include "Factories/Factory.h"
#include "Misc/TransactionObjectEvent.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "UObject/ObjectSaveContext.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> UBangoLevelScriptsEditorSubsystem::Filter;


UBangoLevelScriptsEditorSubsystem* UBangoLevelScriptsEditorSubsystem::Get()
{
	return GEditor->GetEditorSubsystem<UBangoLevelScriptsEditorSubsystem>();
}

void UBangoLevelScriptsEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UContentBrowserDataSubsystem>();

	Filter = MakeShared<FBangoHideScriptFolderFilter>();
	UContentBrowserDataSubsystem* ContentBrowserData = IContentBrowserDataModule::Get().GetSubsystem();
	ContentBrowserData->RegisterCreateHideFolderIfEmptyFilter(FContentBrowserCreateHideFolderIfEmptyFilter::CreateLambda([this] () { return Filter; }));

	GEngine->OnLevelActorAdded().AddUObject(this, &ThisClass::OnLevelActorAdded);
	GEngine->OnLevelActorDeleted().AddUObject(this, &ThisClass::OnLevelActorDeleted);
	
	FEditorDelegates::OnMapLoad.AddUObject(this, &ThisClass::OnMapLoad);
	FEditorDelegates::OnMapOpened.AddUObject(this, &ThisClass::OnMapOpened);
	
	FEditorDelegates::PreSaveWorldWithContext.AddUObject(this, &ThisClass::PreSaveWorldWithContext);
	
	FEditorDelegates::OnDuplicateActorsBegin.AddUObject(this, &ThisClass::OnDuplicateActorsBegin);
	FEditorDelegates::OnDuplicateActorsEnd.AddUObject(this, &ThisClass::OnDuplicateActorsEnd);
	FEditorDelegates::OnDeleteActorsBegin.AddUObject(this, &ThisClass::OnDeleteActorsBegin);
	FEditorDelegates::OnDeleteActorsEnd.AddUObject(this, &ThisClass::OnDeleteActorsEnd);
	
	FCoreUObjectDelegates::OnObjectPreSave.AddUObject(this, &ThisClass::OnObjectPreSave);
	
	FBangoEditorDelegates::OnScriptContainerCreated.AddUObject(this, &ThisClass::OnLevelScriptContainerCreated);
	FBangoEditorDelegates::OnScriptContainerDestroyed.AddUObject(this, &ThisClass::OnLevelScriptContainerDestroyed);
	FBangoEditorDelegates::OnScriptContainerUnregisteredDuringTransaction.AddUObject(this, &ThisClass::OnLevelScriptContainerUnregisteredDuringTransaction);
	FBangoEditorDelegates::OnScriptContainerDuplicated.AddUObject(this, &ThisClass::OnLevelScriptContainerDuplicated);
	
	// TODO am I deprecating my ID system, do I need this?
	FBangoEditorDelegates::RequestNewID.AddUObject(this, &ThisClass::OnRequestNewID);
	
	FCoreUObjectDelegates::OnObjectRenamed.AddUObject(this, &ThisClass::OnObjectRenamed);
	FCoreUObjectDelegates::OnObjectTransacted.AddUObject(this, &ThisClass::OnObjectTransacted);
}

FString UBangoLevelScriptsEditorSubsystem::GetState(UObject* Object) const
{
	bool bIsTemplate = Object->IsTemplate();
	bool bIsValid = IsValid(Object);
	bool bMirroredGarbage = Object->HasAllFlags(RF_MirroredGarbage);
	bool bGarbage = Object->HasAnyInternalFlags(EInternalObjectFlags::Garbage);
	bool bUnreachable = Object->HasAnyInternalFlags(EInternalObjectFlags::Unreachable);
	bool bPendingConstruction = Object->HasAnyInternalFlags(EInternalObjectFlags::PendingConstruction);
	
	return FString::Format(TEXT("Duplicating: {0}, Template {1}, IsValid {2}, MirroredGarbage {3}, Garbage {4}, Unreachable {5}, PendingConstruction {6}"), { (uint8)bDuplicateActorsActive, (uint8)bIsTemplate, (uint8)bIsValid, (uint8)bMirroredGarbage, (uint8)bGarbage, (uint8)bUnreachable, (uint8)bPendingConstruction} );
}

void UBangoLevelScriptsEditorSubsystem::OnObjectPreSave(UObject* Object, FObjectPreSaveContext ObjectPreSaveContext) const
{
	//UE_LOG(LogBango, Display, TEXT("OnObjectPreSave %s --- %s"), *Object->GetName(), *GetState(Object));	
}

void UBangoLevelScriptsEditorSubsystem::OnObjectTransacted(UObject* Object, const class FTransactionObjectEvent& TransactionEvent)
{
	if (TransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo)
	{
		return;
	}
	
	// TODO build a setup to register other class types for undo handling
	UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(Object);
	
	// do NOT check IsValid here. Undoing creation will have an invalid object.
	if (!ScriptComponent)
	{
		return;
	}
	
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnObjectTransacted: %s, %i"), *Object->GetName(), (uint8)TransactionEvent.GetEventType());
	
	// TODO dismantle mount everest below
	if (!Bango::Editor::IsComponentInEditedLevel(ScriptComponent))
	{
		EnqueueDestroyedScriptComponent(Object, &ScriptComponent->ScriptContainer);
	}
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnAssetPostImport(UFactory* Factory, UObject* Object) const
{
	FString FactoryString = Factory ? Factory->GetName() : "No Factory";
	
	FString ObjectString = Object ? Object->GetName() : "No Super";

	//UE_LOG(LogBango, Display, TEXT("OnAssetPostImport %s %s --- %s"), *FactoryString, *ObjectString, *GetState(Object));
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnPackageDeleted(UPackage* Package) const
{
	//UE_LOG(LogBango, Display, TEXT("OnPackageDeleted: %s"), *Package->GetName());
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnAssetsAddExtraObjectsToDelete(TArray<UObject*>& Objects) const
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetsAddExtraObjectsToDelete:"));
	
	for (auto X : Objects)
	{
		//UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnAssetsPreDelete(const TArray<UObject*>& Objects) const
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetsPreDelete:"));
	
	for (auto X : Objects)
	{
		//UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnAssetsDeleted(const TArray<UClass*>& Classes)
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetsDeleted:"));
	
	for (auto X : Classes)
	{
		//UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

void UBangoLevelScriptsEditorSubsystem::OnDuplicateActorsBegin()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsBegin"));
	bDuplicateActorsActive = true;
}

void UBangoLevelScriptsEditorSubsystem::OnDuplicateActorsEnd()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsEnd"));
	bDuplicateActorsActive = false;
}

void UBangoLevelScriptsEditorSubsystem::OnDeleteActorsBegin()
{
	UE_LOG(LogBango, Display, TEXT("OnDeleteActorsBegin"));
	bDeleteActorsActive = true;	
}

void UBangoLevelScriptsEditorSubsystem::OnDeleteActorsEnd()
{
	UE_LOG(LogBango, Display, TEXT("OnDeleteActorsEnd"));
	bDeleteActorsActive = false;
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnLevelActorAdded(AActor* Actor) const
{
	UE_LOG(LogBango, Display, TEXT("OnLevelActorAdded: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnLevelActorDeleted(AActor* Actor) const
{
	UE_LOG(LogBango, Display, TEXT("OnLevelActorDeleted: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
	
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(Comps);
	
	for (auto X : Comps)
	{
		//UE_LOG(LogBango, Display, TEXT("Deleted comp... %s"), *(X->GetName()));
	}
}

void UBangoLevelScriptsEditorSubsystem::OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap)
{
	bMapLoading = true;
	
	// TODO check if I need to do this. The intent is to obliterate "undo" soft delete script assets.
	auto DelayCollectGarbage = FTimerDelegate::CreateLambda([] ()
	{
		CollectGarbage(RF_NoFlags);	
	});
	
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayCollectGarbage);
}

void UBangoLevelScriptsEditorSubsystem::OnMapOpened(const FString& String, bool bArg)
{
	bMapLoading = false;
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::PreSaveWorldWithContext(UWorld* World, FObjectPreSaveContext ObjectPreSaveContext) const
{
	//UE_LOG(LogBango, Display, TEXT("PreSaveWorldWithContext"));
	//ObjectTools::ForceDeleteObjects( { ScriptPackage }, true);
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnObjectConstructed(UObject* Object) const
{
	//UE_LOG(LogBango, Display, TEXT("OnObjectConstructed: %s --- %s"), *Object->GetName(), *GetState(Object));
}

void UBangoLevelScriptsEditorSubsystem::OnObjectRenamed(UObject* RenamedObject, UObject* RenamedObjectOuter, FName OldName) const
{
	// This happens when a copied component is pasted
	if (RenamedObject->GetFlags() == RF_Transactional)
	{
		return;
	}
	
	TWeakObjectPtr<UObject> WeakRenamedObject = RenamedObject;
	TWeakObjectPtr<UObject> WeakRenamedObjectOuter = RenamedObjectOuter;
	TWeakObjectPtr<const UBangoLevelScriptsEditorSubsystem> WeakThis = this;
	
	auto DelayedScriptRename = [WeakRenamedObject, WeakRenamedObjectOuter, WeakThis, OldName] ()
	{
		UObject* RenamedObject = WeakRenamedObject.Get();
		UObject* RenamedObjectOuter = WeakRenamedObjectOuter.Get();
		const UBangoLevelScriptsEditorSubsystem* This = WeakThis.Get();
		
		if (!RenamedObject || !RenamedObjectOuter || !This)
		{
			return;
		}
		
		// TODO I need a way to allow hooking in other types more nicely. What if a user wants a custom type and not just UBangoScriptComponent?
		if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(RenamedObject))
		{
			if (!IsValid(RenamedObject) || RenamedObject->HasAnyFlags(RF_ArchetypeObject) || !Bango::Editor::IsComponentInEditedLevel(ScriptComponent))
			{
				return;
			}
	
			UE_LOG(LogBangoEditor, Verbose, TEXT("OnObjectRenamed: %s, %s, %s"), *RenamedObject->GetName(), *RenamedObjectOuter->GetName(), *OldName.ToString());
			
			UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptComponent->ScriptContainer.GetScriptClass());

			if (Blueprint)
			{
				// This is creating rediretors to my script assets below. Do I want it to? They're hard to get rid of for some stupid reason. Seems like it'd be easier to just rename & fixup always.
				//FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
				//FAssetRenameData RenameData(Blueprint, FPackageName::GetLongPackagePath(Blueprint->GetPackage()->GetPathName()), ScriptComponent->GetName());
				//AssetToolsModule.Get().RenameAssets( {RenameData} );
			
				(void)Blueprint->MarkPackageDirty();
				
				FString NewPrivateName = Bango::Editor::GetLocalScriptName(ScriptComponent->GetName());
				Blueprint->Rename(*NewPrivateName, nullptr, REN_DontCreateRedirectors | REN_NonTransactional);

				Blueprint->Modify();
				FKismetEditorUtilities::CompileBlueprint(Blueprint);
				
				ScriptComponent->Modify();
				ScriptComponent->ScriptContainer.SetScriptClass(Blueprint->GeneratedClass);
				
				// Tell details panels / script component customization to refresh
				This->OnScriptGenerated.Broadcast();
			}
		}
	};
	
#if 0
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptClass); 
	
	UPackage* OldPackage = Blueprint->GetPackage();
	
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(Blueprint);

	Blueprint->ClearEditorReferences();
	
	// We're going to "softly" delete the blueprint. Stash its name & package path inside of it, and then rename it to its Guid form.
	// When the script container is restored (undo delete), it can look for the script inside the transient package by its Guid to restore it.
	Blueprint->DeletedName = Blueprint->GetName();
	Blueprint->DeletedPackagePath = Blueprint->GetPackage()->GetPathName();
	Blueprint->DeletedPackagePersistentGuid = OldPackage->GetPersistentGuid();
	Blueprint->DeletedPackageId = OldPackage->GetPackageId();
	Blueprint->Rename(*Blueprint->ScriptGuid.ToString(), GetTransientPackage(), REN_DontCreateRedirectors | REN_DoNotDirty | REN_NonTransactional);
		
	UBangoDummyObject* WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt = NewObject<UBangoDummyObject>(OldPackage);
		
	int32 NumDelete = ObjectTools::ForceDeleteObjects( { WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt }, false );
				
	if (NumDelete == 0)
	{
		ObjectTools::ForceDeleteObjects( { WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt } );
	}

	Bango::Editor::DeleteEmptyLevelScriptFolders();
#endif
	
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayedScriptRename);
	//UE_LOG(LogBango, Display, TEXT("OnObjectRenamed: %s  %s  %s --- %s"), *OuterString, *ObjectString, *OldName.ToString(), *GetState(RenamedObjectOuter));
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnAssetLoaded(UObject* Object) const
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetLoaded: %s --- %s"), *Object->GetName(), *GetState(Object));
}

// TODO erase
void UBangoLevelScriptsEditorSubsystem::OnObjectModified(UObject* Object) const
{
	//UE_LOG(LogBango, Display, TEXT("OnObjectModified: %s --- %s"), *Object->GetName(), *GetState(Object));
}

void UBangoLevelScriptsEditorSubsystem::OnLevelScriptContainerCreated(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString BlueprintName)
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerCreated: %s, %s, %s"), *Outer->GetName(), *ScriptContainer->GetGuid().ToString(), *BlueprintName);

	ScriptContainer->SetRequestedName(BlueprintName);
	
	EnqueueCreatedScriptComponent(Outer, ScriptContainer);
	
#if 0
	
	if (bDuplicateActorsActive)
	{
		// TODO
		//OnScriptContainerDuplicated(Outer, ScriptContainer);
		return;
	}
	
	TWeakObjectPtr<UBangoLevelScriptsEditorSubsystem> WeakThis = this;
	TWeakObjectPtr<UObject> WeakOuter = Outer;
	
	auto Delayed = FTimerDelegate::CreateLambda([WeakThis, WeakOuter, ScriptContainer, BlueprintName] ()
	{
		if (!WeakThis.IsValid() || !WeakOuter.IsValid())
		{
			return;
		}
		
		UBangoLevelScriptsEditorSubsystem* This = WeakThis.Get();
		UObject* Outer = WeakOuter.Get();
		
		if (ScriptContainer->GetGuid().IsValid())
		{
			return;
			/*
			if (!Outer->HasAnyFlags(RF_WasLoaded))
			{
				// This creation is from an undo operation. We destroyed the package before during delete, so make a new one.
				ScriptPackage = Bango::Editor::MakePackageForScript(Outer, NewBlueprintName, ScriptContainer->Guid);
		
				if (!ScriptPackage)
				{
					UE_LOG(LogBango, Error, TEXT("Tried to create a new script but could not create a package!"));
					return;
				}
		
				UBangoScriptBlueprint* FoundBlueprint = nullptr;
				FBangoEditorDelegates::OnBangoActorComponentUndoDelete.Broadcast(ScriptContainer->Guid, FoundBlueprint);
			
				if (FoundBlueprint)
				{
					Blueprint = FoundBlueprint;
					Blueprint->StopListeningForUndelete();
					Blueprint->Rename(*Blueprint->RetrieveDeletedName(), ScriptPackage, REN_DontCreateRedirectors | REN_NonTransactional);
				}
			}
			*/
		}
		else
		{
			UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerCreated: %s"), *Outer->GetName());
	
			// This creation is from a new addition
			Outer->Modify();
			
			// This is storing my script blueprints in __BangoScripts__ packages - this is harder to figure out how to manage reliably and is WIP
			// Pros: similar to OFPA, designers don't have VCS conflicts... and you can have multiple with the same name
			AActor* Actor = Outer->GetTypedOuter<AActor>();
			
			FGuid NewScriptGuid = FGuid::NewGuid(); 
			ScriptContainer->SetGuid(NewScriptGuid);
			
			UPackage* ScriptPackage = Bango::Editor::MakeLevelScriptPackage(Outer, NewScriptGuid);
			
			FString NewBlueprintName = BlueprintName;
			
			if (NewBlueprintName.IsEmpty())
			{
				NewBlueprintName = FPackageName::GetShortName(ScriptPackage);
			}
			
			UBangoScriptBlueprint* Blueprint = Bango::Editor::MakeLevelScript(ScriptPackage, NewBlueprintName, NewScriptGuid);
			
			Blueprint->Modify();
			Blueprint->ActorReference = Actor->GetPathName();
			
			if (Blueprint)
			{
				Blueprint->SetGuid(ScriptContainer->GetGuid());
				
				UClass* GenClass = Blueprint->GeneratedClass;
				
				if (GenClass)
				{
					UObject* CDO = GenClass->GetDefaultObject();
					UBangoScript* BangoScript = Cast<UBangoScript>(CDO);
					
					if (BangoScript)
					{
						BangoScript->SetThis_ClassType(Actor->GetClass());
					}
				}
			
				ScriptContainer->SetScriptClass(Blueprint->GeneratedClass);
			
				FAssetRegistryModule::AssetCreated(Blueprint);
				(void)ScriptPackage->MarkPackageDirty();
			
				// Tells FBangoScript property type customizations to regenerate
				This->OnScriptGenerated.Broadcast();	
			}
		}
	});
	
	GEditor->GetTimerManager()->SetTimerForNextTick(Delayed);
#endif 
}

// TODO - find path for 1) add new component, 2) save, 3) undo add new component --- it leaves the script .uasset file in place instead of deleting it. Undoing an add is not detected as a destroy?
void UBangoLevelScriptsEditorSubsystem::OnLevelScriptContainerDestroyed(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerDestroyed: %s"), *Outer->GetName());

	EnqueueDestroyedScriptComponent(Outer, ScriptContainer);
	
#if 0
	const FSoftObjectPath& ScriptClassPath = ScriptClass.ToSoftObjectPath();
	
	if (ScriptClassPath.IsNull())
	{
		// This can happen if you 1) Add a UBangoScriptComponent to an actor, 2) press Undo (without saving or doing anything else)
		return;
	}
	
	if (!FPackageName::DoesPackageExist(ScriptClassPath.GetLongPackageName()))
	{
		// This *should* never happen
		return;	
	}
	
	SoftDeleteLevelScriptPackage(ScriptClass);
	
	//Bango::Editor::DeleteScriptIfUnreferenced(ScriptClass);
#endif
}

void UBangoLevelScriptsEditorSubsystem::OnLevelScriptContainerUnregisteredDuringTransaction(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptClass)
{
	if (!bDuplicateActorsActive)
	{
		//OnLevelScriptContainerDestroyed(Outer, ScriptClass);
	}
}

/*
bool UBangoLevelScriptsEditorSubsystem::IsExistingScriptContainerValid(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	check(IsValid(Outer));
	check(ScriptContainer);
	
	if (!ScriptContainer->GetScriptClass() || !ScriptContainer->GetGuid().IsValid())
	{
		return false;
	}
	
	return true;
}
*/

void UBangoLevelScriptsEditorSubsystem::OnLevelScriptContainerDuplicated(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString BlueprintName)
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerDuplicated: %s, %s, %s"), *Outer->GetName(), *ScriptContainer->GetGuid().ToString(), *BlueprintName);
	
	ScriptContainer->SetIsDuplicate();
	ScriptContainer->SetRequestedName(BlueprintName);
	
	EnqueueCreatedScriptComponent(Outer, ScriptContainer);
	
#if 0
	// When an actor is added to the world containing a CDO ScriptComponent, this function might be called. 
	TWeakObjectPtr<UObject> WeakOuter = Outer;
	TWeakObjectPtr<UBangoLevelScriptsEditorSubsystem> WeakThis = this;
	
	auto DelayOneFrame = [WeakThis, WeakOuter, ScriptContainer, BlueprintName] ()
	{
		if (!WeakThis.IsValid() || !WeakOuter.IsValid())
		{
			return;
		}
		
		UObject* Outer = WeakOuter.Get();
		UBangoLevelScriptsEditorSubsystem* This = WeakThis.Get();
		
		if (!ScriptContainer->GetScriptClass())
		{
			// Switch over to the >OnScriptContainerCreated path; this code path occurs when you drag a content blueprint into the scene as it "duplicates" the CDO component
			This->OnLevelScriptContainerCreated(Outer, ScriptContainer, BlueprintName);
			return;
		}
		
		UBangoScriptBlueprint* SourceBlueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->GetScriptClass());
		
		// Prepare the newly duplicated script container
		ScriptContainer->Unset();
		
		FGuid NewScriptGuid = FGuid::NewGuid(); 
		ScriptContainer->SetGuid(NewScriptGuid);
		
		// Duplicate the blueprint
		UPackage* NewScriptPackage = Bango::Editor::MakeLevelScriptPackage(Outer, NewScriptGuid);
		
		if (!NewScriptPackage)
		{
			UE_LOG(LogBango, Error, TEXT("Tried to create a new script but could not create a package!"));
			return;
		}
		
		UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerDuplicated: %s, %s, %s"), *Outer->GetName(), *ScriptContainer->GetGuid().ToString(), *BlueprintName);
		
		UBangoScriptBlueprint* NewBlueprint = nullptr;
		
		if (SourceBlueprint)
		{
			NewBlueprint = Bango::Editor::DuplicateLevelScript(SourceBlueprint, NewScriptPackage, BlueprintName, NewScriptGuid);
			NewBlueprint->ActorReference = Outer->GetTypedOuter<AActor>()->GetPathName();	
		}

		if (NewBlueprint)
		{
			ScriptContainer->SetScriptClass(NewBlueprint->GeneratedClass);
	
			FAssetRegistryModule::AssetCreated(NewBlueprint);
			(void)NewScriptPackage->MarkPackageDirty();
	
			// Tells FBangoScript property type customizations to regenerate
			This->OnScriptGenerated.Broadcast();	
		}
	};
	
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayOneFrame);
#endif
}

void UBangoLevelScriptsEditorSubsystem::OnRequestNewID(AActor* Actor) const
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnRequestNewID: %s"), *Actor->GetName());
	FBangoEditorMenus::SetEditActorID(Actor, true);
}

void UBangoLevelScriptsEditorSubsystem::EnqueueCreatedScriptComponent(UObject* Owner, FBangoScriptContainer* ScriptContainer)
{
	if (bMapLoading)
	{
		return;
	}
	
	if (!Owner || !ScriptContainer)
	{
		checkNoEntry();
		return;
	}
	
	UE_LOG(LogBangoEditor, Verbose, TEXT("EnqueueCreatedScriptComponent: %s (%p)"), *Owner->GetName(), ScriptContainer);
	
	FScriptContainerKey Key(Owner, ScriptContainer);
	CreationRequests.Add(Key);
	QueueProcessScriptRequestQueues();
}

void UBangoLevelScriptsEditorSubsystem::EnqueueDestroyedScriptComponent(UObject* Owner, FBangoScriptContainer* ScriptContainer)
{
	if (bMapLoading)
	{
		return;
	}
	
	if (!Owner || !ScriptContainer)
	{
		checkNoEntry();
		return;
	}
	
	if (!ScriptContainer->GetGuid().IsValid())
	{
		return;
	}
	
	if (ScriptContainer->GetScriptClass().IsNull())
	{
		return;
	}
	
	UE_LOG(LogBangoEditor, Verbose, TEXT("EnqueueDestroyedScriptComponent: %s (%p)"), *Owner->GetName(), ScriptContainer);
	
	DestructionRequests.Add(ScriptContainer->GetScriptClass());
	QueueProcessScriptRequestQueues();
}

void UBangoLevelScriptsEditorSubsystem::QueueProcessScriptRequestQueues()
{
	TWeakObjectPtr<UBangoLevelScriptsEditorSubsystem> WeakThis = this;
			
	auto ProcessScriptRequestQueuesNextTick = FTimerDelegate::CreateLambda([WeakThis] ()
	{
		if (WeakThis.IsValid())
		{
			WeakThis->ProcessScriptRequestQueues();
		}
	});
		
	ProcessScriptRequestQueuesHandle = GEditor->GetTimerManager()->SetTimerForNextTick(ProcessScriptRequestQueuesNextTick);
}

void UBangoLevelScriptsEditorSubsystem::ProcessScriptRequestQueues()
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("--------------------------------"))
	UE_LOG(LogBangoEditor, Verbose, TEXT("ProcessScriptRequestQueue"))

	// This should always be running one frame later than the requests were queued
	
	for (const auto& Request : CreationRequests)
	{
		if (!Request.ScriptOuter.IsValid())
		{
			continue;
		}
		
		TSoftClassPtr<UBangoScript> Script = Request.ScriptContainer->GetScriptClass();
		UE_LOG(LogBangoEditor, Verbose, TEXT("     %s"),*Request.ScriptContainer->GetScriptClass().ToString());
		
		// We want creation requests to take priority; parse through creations first and have any creation requests cancel out any destruction requestss
		DestructionRequests.Remove(Script);
		ProcessCreatedScriptRequest(Request.ScriptOuter, Request.ScriptContainer);
	}
	
	for (const TSoftClassPtr<UBangoScript>& DestroyedScript : DestructionRequests)
	{
		ProcessDestroyedScriptRequest(DestroyedScript);
	}
	
	CreationRequests.Empty();
	DestructionRequests.Empty();
	ProcessScriptRequestQueuesHandle.Invalidate();
	
	UE_LOG(LogBangoEditor, Verbose, TEXT("--------------------------------"))
}

void UBangoLevelScriptsEditorSubsystem::ProcessCreatedScriptRequest(TWeakObjectPtr<UObject> Owner, FBangoScriptContainer* ScriptContainer)
{
	if (!Owner.IsValid())
	{
		return;
	}

	TSoftClassPtr<UBangoScript> ExistingScriptClass = ScriptContainer->GetScriptClass();
	
	if (ExistingScriptClass.IsNull())
	{
		// This must be a fresh shiny brand new script container. Make a script for it!
		CreateLevelScript(Owner.Get(), ScriptContainer);
	}
	else
	{
		if (ScriptContainer->ConsumeDuplicate())
		{
			DuplicateLevelScript(Owner.Get(), ScriptContainer);
		}
		else
		{
			// This script container already has a script class assigned, *and* it wasn't a duplicate... 			
			FSoftObjectPath ScriptClassSoft(ScriptContainer->GetScriptClass().ToSoftObjectPath());
					
			bool bScriptExists = FPackageName::DoesPackageExist(ScriptClassSoft.GetLongPackageName());
			
			if (!bScriptExists)
			{
				// This must have been an undo-delete. Let's see if we can find the script class in the Transient Package.
				TryUndeleteScript(ScriptClassSoft, ScriptContainer);
			}
		}
	}
}

void UBangoLevelScriptsEditorSubsystem::ProcessDestroyedScriptRequest(TSoftClassPtr<UBangoScript> ScriptClass)
{
	const FSoftObjectPath& ScriptClassPath = ScriptClass.ToSoftObjectPath();
	
	if (ScriptClassPath.IsNull())
	{
		return;
	}
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptClass); 

	if (!Blueprint)
	{
		UE_LOG(LogBangoEditor, Error, TEXT("SoftDeleteLevelScriptPackage failed - Null Blueprint: %s"), *ScriptClass.ToSoftObjectPath().GetAssetPathString());
		return;
	}
	
	UPackage* OldPackage = Blueprint->GetPackage();

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(Blueprint);

	Blueprint->ClearEditorReferences();

	// We're going to "softly" delete the blueprint. Stash its name & package path inside of it, and then rename it to its Guid form.
	// When the script container is restored (undo delete), it can look for the script inside the transient package by its Guid to restore it.
	Blueprint->DeletedName = Blueprint->GetName();
	Blueprint->DeletedPackagePath = Blueprint->GetPackage()->GetPathName();
	Blueprint->DeletedPackagePersistentGuid = OldPackage->GetPersistentGuid();
	Blueprint->DeletedPackageId = OldPackage->GetPackageId();
	Blueprint->Rename(*Blueprint->ScriptGuid.ToString(), GetTransientPackage(), REN_DontCreateRedirectors | REN_DoNotDirty | REN_NonTransactional);
	
	// TODO Is there another way to make this work without this? I can't successfully run ObjectTools' delete funcs on a UPackage, only on an asset inside a UPackage!
	UBangoDummyObject* WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt = NewObject<UBangoDummyObject>(OldPackage);
	
	int32 NumDelete = ObjectTools::ForceDeleteObjects( { WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt }, false );
			
	if (NumDelete == 0)
	{
		ObjectTools::ForceDeleteObjects( { WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt } );
	}

	Bango::Editor::DeleteEmptyLevelScriptFolders();
}

void UBangoLevelScriptsEditorSubsystem::CreateLevelScript(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	// This function must be ran one frame *after* the script container is created. This gives UE time finish all serialization behavior.
	
	if (ScriptContainer->GetGuid().IsValid())
	{
		// We don't want to create a new script if it already has one
		return;
	}
	else
	{
		Outer->Modify();
		
		// All level scripts are supposed to be tied to an actor, although it doesn't actually do anything at runtime, it is moreso an editor concept.
		AActor* Actor = Outer->GetTypedOuter<AActor>();
		
		FGuid NewScriptGuid = FGuid::NewGuid(); 
		ScriptContainer->SetGuid(NewScriptGuid);
		
		UPackage* ScriptPackage = Bango::Editor::MakeLevelScriptPackage(Outer, NewScriptGuid);
		
		if (!ScriptPackage)
		{
			UE_LOG(LogBangoEditor, Error, TEXT("Failed to create package for new Bango level script asset! Perhaps a file system issue?"));
			return;
		}

		UE_LOG(LogBangoEditor, Display, TEXT("Created new Bango level script asset: %s"), *ScriptPackage->GetPathName());
		
		// Higher-level systems can fill in the requested name to get it passed in here for creation. This was a bit of a late-stage hack, not very "clean code", but it works fine
		FString NewBlueprintName = ScriptContainer->GetRequestedName();
		
		if (NewBlueprintName.IsEmpty())
		{
			NewBlueprintName = FPackageName::GetShortName(ScriptPackage);
		}
		
		// Make the actual script blueprint asset!
		UBangoScriptBlueprint* Blueprint = Bango::Editor::MakeLevelScript(ScriptPackage, NewBlueprintName, NewScriptGuid);
		
		Blueprint->Modify();
		Blueprint->ActorReference = Actor->GetPathName();
		
		if (!Blueprint)
		{
			UE_LOG(LogBangoEditor, Error, TEXT("Failed to create new level script!"))
			return;
		}
	
		Blueprint->SetGuid(ScriptContainer->GetGuid());
		
		UClass* GenClass = Blueprint->GeneratedClass;
		
		if (GenClass)
		{
			UObject* CDO = GenClass->GetDefaultObject();
			UBangoScript* BangoScript = Cast<UBangoScript>(CDO);
			
			if (BangoScript)
			{
				// This enables the "This" node to self-setup its own output cast type
				BangoScript->SetThis_ClassType(Actor->GetClass());
			}
		}
	
		ScriptContainer->SetScriptClass(Blueprint->GeneratedClass);
	
		FKismetEditorUtilities::CompileBlueprint(Blueprint);
		FAssetRegistryModule::AssetCreated(Blueprint);
		(void)ScriptPackage->MarkPackageDirty();
	
		// Tells FBangoScript property type customizations to regenerate their view
		OnScriptGenerated.Broadcast();	
	}
}

void UBangoLevelScriptsEditorSubsystem::DuplicateLevelScript(UObject* Owner, FBangoScriptContainer* ScriptContainer)
{
	UBangoScriptBlueprint* SourceBlueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->GetScriptClass());
		
	// Prepare the newly duplicated script container
	ScriptContainer->Unset();
		
	FGuid NewScriptGuid = FGuid::NewGuid(); 
	ScriptContainer->SetGuid(NewScriptGuid);
		
	// Duplicate the blueprint
	UPackage* NewScriptPackage = Bango::Editor::MakeLevelScriptPackage(Owner, NewScriptGuid);
		
	if (!NewScriptPackage)
	{
		UE_LOG(LogBango, Error, TEXT("Tried to create a new script but could not create a package!"));
		return;
	}
		
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerDuplicated: %s, %s, %s"), *Owner->GetName(), *ScriptContainer->GetGuid().ToString(), *ScriptContainer->GetRequestedName());
		
	UBangoScriptBlueprint* DuplicatedBlueprint = nullptr;
		
	if (SourceBlueprint)
	{
		DuplicatedBlueprint = Bango::Editor::DuplicateLevelScript(SourceBlueprint, NewScriptPackage, ScriptContainer->GetRequestedName(), NewScriptGuid);
		DuplicatedBlueprint->ActorReference = Owner->GetTypedOuter<AActor>()->GetPathName();	
	}

	if (DuplicatedBlueprint)
	{
		ScriptContainer->SetScriptClass(DuplicatedBlueprint->GeneratedClass);
	
		FKismetEditorUtilities::CompileBlueprint(DuplicatedBlueprint);
				
		FAssetRegistryModule::AssetCreated(DuplicatedBlueprint);
		(void)NewScriptPackage->MarkPackageDirty();
	
		// Tells FBangoScript property type customizations to regenerate
		OnScriptGenerated.Broadcast();	
	}
}

void UBangoLevelScriptsEditorSubsystem::TryUndeleteScript(FSoftObjectPath ScriptClassSoft, FBangoScriptContainer* ScriptContainer)
{
	TArray<UObject*> TransientObjects;
    GetObjectsWithOuter(GetTransientPackage(), TransientObjects);

    UBangoScriptBlueprint* MatchedBlueprint = nullptr;
    
    for (UObject* TransientObject : TransientObjects)
    {
        if (UBangoScriptBlueprint* TransientBangoScriptBlueprint = Cast<UBangoScriptBlueprint>(TransientObject))
        {
            if (TransientBangoScriptBlueprint->DeletedPackagePath == ScriptClassSoft.GetAssetPath().GetPackageName().ToString())
            {
                if (FPackageName::DoesPackageExist(TransientBangoScriptBlueprint->DeletedPackagePath.GetLongPackageName()))
                {
                	UE_LOG(LogBango, Warning, TEXT("Tried to restore deleted Bango Blueprint but there was another package at the location. Invalidating this Script Container property."));
                	ScriptContainer->Unset();
                }
                else
                {
                	MatchedBlueprint = TransientBangoScriptBlueprint;
                	break;
                }
            }
        }
    }
    
    if (MatchedBlueprint)
    {
        UPackage* RestoredPackage = CreatePackage(*MatchedBlueprint->DeletedPackagePath.ToString());
        RestoredPackage->SetFlags(RF_Public);
        RestoredPackage->SetPackageFlags(PKG_NewlyCreated);
        RestoredPackage->SetPersistentGuid(MatchedBlueprint->DeletedPackagePersistentGuid);
        RestoredPackage->SetPackageId(MatchedBlueprint->DeletedPackageId);
                			
        MatchedBlueprint->Rename(*MatchedBlueprint->DeletedName, RestoredPackage, REN_DontCreateRedirectors | REN_DoNotDirty | REN_NonTransactional);
        MatchedBlueprint->Modify();
        MatchedBlueprint->ClearFlags(RF_Transient);
                			
        FAssetRegistryModule::AssetCreated(MatchedBlueprint);
        (void)MatchedBlueprint->MarkPackageDirty();
        
        if (RestoredPackage->GetPackageId().IsValid())
        {
            GEditor->GetEditorSubsystem<UEditorAssetSubsystem>()->SaveLoadedAsset(MatchedBlueprint, false);
        }
    }
    else
    {
        UE_LOG(LogBangoEditor, Warning, TEXT("Error - could not find associated blueprint for %s"), *ScriptClassSoft.ToString());
    }
}

#undef LOCTEXT_NAMESPACE
