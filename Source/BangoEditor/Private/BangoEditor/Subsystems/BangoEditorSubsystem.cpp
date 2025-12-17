#include "BangoEditorSubsystem.h"

#include "ContentBrowserDataSubsystem.h"
#include "IContentBrowserDataModule.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScript.h"
#include "Bango/Core/BangoScriptContainer.h"
#include "Bango/Editor/BangoDebugUtility.h"
#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/DevTesting/BangoDummyObject.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "Helpers/BangoHideScriptFolderFilter.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"

TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> UBangoEditorSubsystem::Filter;


UBangoEditorSubsystem* UBangoEditorSubsystem::Get()
{
	return GEditor->GetEditorSubsystem<UBangoEditorSubsystem>();
}

void UBangoEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UContentBrowserDataSubsystem>();

	Filter = MakeShared<FBangoHideScriptFolderFilter>();
	UContentBrowserDataSubsystem* ContentBrowserData = IContentBrowserDataModule::Get().GetSubsystem();
	ContentBrowserData->RegisterCreateHideFolderIfEmptyFilter(FContentBrowserCreateHideFolderIfEmptyFilter::CreateLambda([this] () { return Filter; }));

	GEngine->OnLevelActorAdded().AddUObject(this, &ThisClass::OnLevelActorAdded);
	GEngine->OnLevelActorDeleted().AddUObject(this, &ThisClass::OnLevelActorDeleted);

	/*
	FEditorDelegates::OnAssetPostImport.AddUObject(this, &ThisClass::OnAssetPostImport);
	FEditorDelegates::OnPackageDeleted.AddUObject(this, &ThisClass::OnPackageDeleted);
	FEditorDelegates::OnAssetsAddExtraObjectsToDelete.AddUObject(this, &ThisClass::OnAssetsAddExtraObjectsToDelete);
	FEditorDelegates::OnAssetsPreDelete.AddUObject(this, &ThisClass::OnAssetsPreDelete);
	FEditorDelegates::OnAssetsDeleted.AddUObject(this, &ThisClass::OnAssetsDeleted);
	FEditorDelegates::OnDuplicateActorsBegin.AddUObject(this, &ThisClass::OnDuplicateActorsBegin);
	FEditorDelegates::OnDuplicateActorsEnd.AddUObject(this, &ThisClass::OnDuplicateActorsEnd);
	FCoreUObjectDelegates::OnObjectConstructed.AddUObject(this, &ThisClass::OnObjectConstructed);
	FCoreUObjectDelegates::OnObjectRenamed.AddUObject(this, &ThisClass::OnObjectRenamed);
	FCoreUObjectDelegates::OnAssetLoaded.AddUObject(this, &ThisClass::OnAssetLoaded);
	FCoreUObjectDelegates::OnObjectModified.AddUObject(this, &ThisClass::OnObjectModified);
	*/
	
	FCoreUObjectDelegates::OnObjectPreSave.AddUObject(this, &ThisClass::OnObjectPreSave);
	
	//GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentCreated.AddUObject(this, &ThisClass::OnScriptComponentCreated);
	//GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentDestroyed.AddUObject(this, &ThisClass::OnScriptComponentDestroyed);
	
	FBangoEditorDelegates::OnScriptContainerCreated.AddUObject(this, &ThisClass::OnScriptContainerCreated);
	FBangoEditorDelegates::OnScriptContainerDestroyed.AddUObject(this, &ThisClass::OnScriptContainerDestroyed);
	FBangoEditorDelegates::OnScriptContainerDuplicated.AddUObject(this, &ThisClass::OnScriptContainerDuplicated);
	
	FCoreUObjectDelegates::OnObjectRenamed.AddUObject(this, &ThisClass::OnObjectRenamed);
}

FString UBangoEditorSubsystem::GetState(UObject* Object) const
{
	bool bIsTemplate = Object->IsTemplate();
	bool bIsValid = IsValid(Object);
	bool bMirroredGarbage = Object->HasAllFlags(RF_MirroredGarbage);
	bool bGarbage = Object->HasAnyInternalFlags(EInternalObjectFlags::Garbage);
	bool bUnreachable = Object->HasAnyInternalFlags(EInternalObjectFlags::Unreachable);
	bool bPendingConstruction = Object->HasAnyInternalFlags(EInternalObjectFlags::PendingConstruction);
	
	return FString::Format(TEXT("Duplicating: {0}, Template {1}, IsValid {2}, MirroredGarbage {3}, Garbage {4}, Unreachable {5}, PendingConstruction {6}"), { (uint8)bDuplicateActorsActive, (uint8)bIsTemplate, (uint8)bIsValid, (uint8)bMirroredGarbage, (uint8)bGarbage, (uint8)bUnreachable, (uint8)bPendingConstruction} );
}

void UBangoEditorSubsystem::OnObjectPreSave(UObject* Object, FObjectPreSaveContext ObjectPreSaveContext) const
{
	//UE_LOG(LogBango, Display, TEXT("OnObjectPreSave %s --- %s"), *Object->GetName(), *GetState(Object));	
}

void UBangoEditorSubsystem::OnAssetPostImport(UFactory* Factory, UObject* Object) const
{
	FString FactoryString = Factory ? Factory->GetName() : "No Factory";
	
	FString ObjectString = Object ? Object->GetName() : "No Super";

	UE_LOG(LogBango, Display, TEXT("OnAssetPostImport %s %s --- %s"), *FactoryString, *ObjectString, *GetState(Object));
}

void UBangoEditorSubsystem::OnPackageDeleted(UPackage* Package) const
{
	UE_LOG(LogBango, Display, TEXT("OnPackageDeleted: %s"), *Package->GetName());
}

void UBangoEditorSubsystem::OnAssetsAddExtraObjectsToDelete(TArray<UObject*>& Objects) const
{
	UE_LOG(LogBango, Display, TEXT("OnAssetsAddExtraObjectsToDelete:"));
	
	for (auto X : Objects)
	{
		UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

void UBangoEditorSubsystem::OnAssetsPreDelete(const TArray<UObject*>& Objects) const
{
	UE_LOG(LogBango, Display, TEXT("OnAssetsPreDelete:"));
	
	for (auto X : Objects)
	{
		UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

void UBangoEditorSubsystem::OnAssetsDeleted(const TArray<UClass*>& Classes)
{
	UE_LOG(LogBango, Display, TEXT("OnAssetsDeleted:"));
	
	for (auto X : Classes)
	{
		UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

void UBangoEditorSubsystem::OnDuplicateActorsBegin()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsBegin"));
	bDuplicateActorsActive = true;
}

void UBangoEditorSubsystem::OnDuplicateActorsEnd()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsEnd"));
	bDuplicateActorsActive = false;
}

void UBangoEditorSubsystem::OnLevelActorAdded(AActor* Actor) const
{
	UE_LOG(LogBango, Display, TEXT("OnLevelActorAdded: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
}

void UBangoEditorSubsystem::OnLevelActorDeleted(AActor* Actor) const
{
	UE_LOG(LogBango, Display, TEXT("OnLevelActorDeleted: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
	
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(Comps);
	
	for (auto X : Comps)
	{
		UE_LOG(LogBango, Display, TEXT("Deleted comp... %s"), *(X->GetName()));
	}
}

void UBangoEditorSubsystem::OnObjectConstructed(UObject* Object) const
{
	UE_LOG(LogBango, Display, TEXT("OnObjectConstructed: %s --- %s"), *Object->GetName(), *GetState(Object));
}

void UBangoEditorSubsystem::OnObjectRenamed(UObject* RenamedObject, UObject* RenamedObjectOuter, FName OldName) const
{
	if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(RenamedObject))
	{
		if (Bango::IsComponentInEditedLevel(ScriptComponent))
		{
			ScriptComponent->OnRename();
			OnScriptGenerated.Broadcast();
		}
	}
	
	//UE_LOG(LogBango, Display, TEXT("OnObjectRenamed: %s  %s  %s --- %s"), *OuterString, *ObjectString, *OldName.ToString(), *GetState(RenamedObjectOuter));
}

void UBangoEditorSubsystem::OnAssetLoaded(UObject* Object) const
{
	UE_LOG(LogBango, Display, TEXT("OnAssetLoaded: %s --- %s"), *Object->GetName(), *GetState(Object));
}

void UBangoEditorSubsystem::OnObjectModified(UObject* Object) const
{
	UE_LOG(LogBango, Display, TEXT("OnObjectModified: %s --- %s"), *Object->GetName(), *GetState(Object));
}

void CheckComponentOrigin(UActorComponent* Component)
{
	if (!Component)
	{
		return;
	}
	
	if (Component->IsDefaultSubobject())
	{
		UE_LOG(LogTemp, Log, TEXT("Component created via CreateDefaultSubobject in C++"));
	}
	else if (Component->CreationMethod == EComponentCreationMethod::Instance)
	{
		UE_LOG(LogTemp, Log, TEXT("Component created via NewObject or added in Level Editor"));
	}
	else if (Component->CreationMethod == EComponentCreationMethod::SimpleConstructionScript)
	{
		UE_LOG(LogTemp, Log, TEXT("Component added in Blueprint editor"));
	}
	else if (Component->CreationMethod == EComponentCreationMethod::UserConstructionScript)
	{
		UE_LOG(LogTemp, Log, TEXT("Component created in Blueprint Construction Script"));
	}
}

void UBangoEditorSubsystem::OnScriptContainerCreated(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	check(IsValid(Outer));
	check(ScriptContainer);
	
	FString NewBlueprintName;
	UPackage* ScriptPackage = nullptr;
	UBangoScriptBlueprint* Blueprint;
	
	if (ScriptContainer->Guid.IsValid())
	{
		ScriptPackage = Bango::Editor::MakePackageForScript(Outer, NewBlueprintName, ScriptContainer->Guid);
	
		if (!ScriptPackage)
		{
			UE_LOG(LogBango, Error, TEXT("Tried to create a new script but could not create a package!"));
			return;
		}
	
		// This creation is from an undo operation
		UBangoEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>();
		Blueprint = Subsystem->RetrieveDeletedScript(ScriptContainer->Guid);
		check(Blueprint);
		
		Blueprint->Rename(nullptr, ScriptPackage, REN_DontCreateRedirectors | REN_NonTransactional);
	}
	else
	{
		ScriptContainer->Guid = FGuid::NewGuid();
		
		ScriptPackage = Bango::Editor::MakePackageForScript(Outer, NewBlueprintName, ScriptContainer->Guid);
		
		FString BPName = UBangoScriptBlueprint::GetAutomaticName(Outer);
		
		// This creation is from a new addition
		Blueprint = Bango::Editor::MakeScriptAsset(ScriptPackage, BPName , ScriptContainer->Guid);
		Blueprint->SetGuid(ScriptContainer->Guid);
		check(Blueprint);
	}

	ScriptContainer->ScriptClass = Blueprint->GeneratedClass;
	
	FAssetRegistryModule::AssetCreated(Blueprint);
	(void)ScriptPackage->MarkPackageDirty();
	
	// Tells FBangoScript property type customizations to regenerate
	OnScriptGenerated.Broadcast();
}

bool IsExistingScriptContainerValid(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	check(IsValid(Outer));
	check(ScriptContainer);
	
	if (!ScriptContainer->ScriptClass || !ScriptContainer->Guid.IsValid())
	{
		return false;
	}
	
	return true;
}

void UBangoEditorSubsystem::OnScriptContainerDestroyed(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	if (!IsExistingScriptContainerValid(Outer, ScriptContainer))
	{
		return;
	}
	
	UAssetEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

	if (IsValid(Subsystem))
	{
		UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->ScriptClass);

		if (Blueprint)
		{
			Subsystem->CloseAllEditorsForAsset(Blueprint);
		}
	}
		
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->ScriptClass); 
	check(Blueprint);
	
	UPackage* ScriptPackage = Blueprint->GetPackage();
	check(ScriptPackage);
		
#if 0
	/*
	// Duplicate the blueprint in the transient package and stash it.
	UBangoScriptBlueprint* BlueprintCopy = DuplicateObject(Blueprint, GetTransientPackage());
	BlueprintCopy->ListenForUndelete(ScriptContainer->Guid); // TODO the blueprint holds the Guid I shouldn't need to pass it in
	SoftDeletedScripts.Add( { ScriptContainer->Guid, TStrongObjectPtr<UBangoScriptBlueprint>(BlueprintCopy) } );
	*/

	Outer->Modify();
	ScriptContainer->Unset();
	(void)Outer->MarkPackageDirty();
	
	Blueprint->ClearEditorReferences();
	
	ResetLoaders(ScriptPackage);
	CollectGarbage(RF_NoFlags);
#endif
	
//#if 0
	// The editor's deletion systems are async and run GC during the process
	//ScriptPackage->AddToRoot();
	
	// Move the blueprint out into transient space. Store it in this subsystem instead for lookup and GC prevention.
	DeletedScripts.Add( { ScriptContainer->Guid, TStrongObjectPtr<UBangoScriptBlueprint>(Blueprint) } );
	Blueprint->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
	Blueprint->ListenForUndelete();
	
	ScriptContainer->Unset();
	Blueprint->ClearEditorReferences();
	
	//UBangoDummyObject* DummyObject = NewObject<UBangoDummyObject>(ScriptPackage, UBangoDummyObject::StaticClass());
	//DummyObject->AddToRoot();
	(void)ScriptPackage->MarkPackageDirty();
	
	//ResetLoaders(ScriptPackage);
	//CollectGarbage(RF_NoFlags);
//#endif
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	// I don't know if this helps anything. Probably not.
//#if 0	
	ScriptPackage->SetDirtyFlag(true);
	
	// Re-scan the asset file on disk to ensure that the updated entry will be based on the serialized FiB tag.
	const FString PackageName = ScriptPackage->GetPathName();
	if (FPackageName::IsValidLongPackageName(PackageName))
	{
		FPackagePath PackagePath;
		if (FPackagePath::TryFromPackageName(PackageName, PackagePath))
		{
			FPackagePath OutPackagePath;
			const FPackageName::EPackageLocationFilter PackageLocation = FPackageName::DoesPackageExistEx(PackagePath, FPackageName::EPackageLocationFilter::Any, /*bMatchCaseOnDisk*/ false, &OutPackagePath);
			if (PackageLocation != FPackageName::EPackageLocationFilter::None)
			{
				if (PackageLocation == FPackageName::EPackageLocationFilter::FileSystem && OutPackagePath.HasLocalPath())
				{
					TArray<FString> FilesToScan = { OutPackagePath.GetLocalFullPath() };
					AssetRegistryModule.Get().ScanModifiedAssetFiles(FilesToScan, UE::AssetRegistry::EScanFlags::ForceRescan);
				}
			}
		}
	}
//#endif
	
	/*
	auto Lambda = FTimerDelegate::CreateLambda([ScriptPackage, Blueprint, DummyObject] ()
	{
		int32 Deleted = ObjectTools::DeleteObjects( { DummyObject }, false );
		ScriptPackage->RemoveFromRoot();
		UE_LOG(LogBango, Display, TEXT("Deleted %i script packages"), Deleted);
	});
	*/
	
	// TODO I can't figure out how to get the stupid asset manager to update IMMEDIATELY. I can't delete the package on the same frame.
	//GEditor->GetTimerManager()->SetTimerForNextTick(Lambda);
}

void UBangoEditorSubsystem::OnScriptContainerDuplicated(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	auto DelayOneFrame = [this, Outer, ScriptContainer] ()
	{
		check(IsValid(Outer));
		check(ScriptContainer);
	
		// Stash the referenced blueprint
		UBangoScriptBlueprint* OldBlueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->ScriptClass);
		
		// Set up this new duplicated script container
		ScriptContainer->Unset();
		ScriptContainer->Guid = FGuid::NewGuid();		
		
		// Dupe the blueprint
		FString NewBlueprintName;
		UPackage* NewScriptPackage = Bango::Editor::MakePackageForScript(Outer, NewBlueprintName, ScriptContainer->Guid);
	
		if (!NewScriptPackage)
		{
			UE_LOG(LogBango, Error, TEXT("Tried to create a new script but could not create a package!"));
			return;
		}
		
		UBangoScriptBlueprint* Blueprint = nullptr;
			
		if (OldBlueprint)
		{
			Blueprint = DuplicateObject(OldBlueprint, NewScriptPackage, FName(NewBlueprintName));
		}

		if (Blueprint)
		{
			ScriptContainer->ScriptClass = Blueprint->GeneratedClass;
	
			FAssetRegistryModule::AssetCreated(Blueprint);
			(void)NewScriptPackage->MarkPackageDirty();
	
			// Tells FBangoScript property type customizations to regenerate
			OnScriptGenerated.Broadcast();	
		}
	};
	
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayOneFrame);
	
	/*
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->ScriptClass);
	
	FString BPName;
	UPackage* PackageForDuplicate = Bango::Editor::MakePackageForScript(Outer, BPName);
	check(PackageForDuplicate);
*/
	
	//FGuid = 
}

void UBangoEditorSubsystem::SoftDeleteScriptPackage(TSubclassOf<UBangoScript> ScriptClass)
{
	UPackage* ScriptPackage = ScriptClass->GetOuterUPackage();
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptClass);
	
	if (!ScriptPackage || !Blueprint)
	{
		return;
	}
	
	TPair<FGuid, TStrongObjectPtr<UBangoScriptBlueprint>> SoftDeletedScript = { ScriptClass->GetDefaultObject<UBangoScript>()->GetScriptGuid(), TStrongObjectPtr<UBangoScriptBlueprint>(Blueprint) };
	DeletedScripts.Add(SoftDeletedScript);
	
	ScriptClass->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty | REN_DontCreateRedirectors);
	
	ObjectTools::DeleteObjects( { ScriptPackage } );
	
	Bango::Editor::DeleteEmptyFolderFromDisk(Bango::Editor::ScriptRootFolder);
	
	//Blueprint->	ListenForUndelete();
}

UBangoScriptBlueprint* UBangoEditorSubsystem::RetrieveDeletedScript(FGuid Guid)
{
	auto Subsystem = Get();
	
	int32 Index = Subsystem->DeletedScripts.IndexOfByPredicate( [Guid] (const TPair<FGuid, TStrongObjectPtr<UBangoScriptBlueprint>>& Element)
	{
		return Element.Key == Guid;
	});
	
	if (Index >= 0)
	{
		TStrongObjectPtr<UBangoScriptBlueprint> Popped = Subsystem->DeletedScripts[Index].Value;
		
		Subsystem->DeletedScripts.RemoveAt(Index);
		
		return Popped.Get();
	}
	
	return nullptr;
}
