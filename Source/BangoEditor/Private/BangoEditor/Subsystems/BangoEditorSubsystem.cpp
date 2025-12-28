#include "BangoEditorSubsystem.h"

#include "ContentBrowserDataSubsystem.h"
#include "FileHelpers.h"
#include "IContentBrowserDataModule.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScript.h"
#include "Bango/Core/BangoScriptContainer.h"
#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/Menus/BangoEditorMenus.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "BangoEditor/Utilities/BangoFolderUtility.h"
#include "Helpers/BangoHideScriptFolderFilter.h"
#include "UObject/ObjectSaveContext.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

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
	
	FEditorDelegates::OnMapLoad.AddUObject(this, &ThisClass::OnMapLoad);
	FEditorDelegates::PreSaveWorldWithContext.AddUObject(this, &ThisClass::PreSaveWorldWithContext);
	
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
	
	FBangoEditorDelegates::RequestNewID.AddUObject(this, &ThisClass::OnRequestNewID);
	
	FCoreUObjectDelegates::OnObjectRenamed.AddUObject(this, &ThisClass::OnObjectRenamed);
	FCoreUObjectDelegates::OnObjectTransacted.AddUObject(this, &ThisClass::OnObjectTransacted);
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

void UBangoEditorSubsystem::OnObjectTransacted(UObject* Object, const class FTransactionObjectEvent& TransactionEvent)
{
	if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(GetValid(Object)))
	{
		if (Bango::IsComponentInEditedLevel(ScriptComponent))
		{
			//FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(ScriptComponent, &ScriptComponent->Script);
		}
	}
}

// TODO erase
void UBangoEditorSubsystem::OnAssetPostImport(UFactory* Factory, UObject* Object) const
{
	FString FactoryString = Factory ? Factory->GetName() : "No Factory";
	
	FString ObjectString = Object ? Object->GetName() : "No Super";

	//UE_LOG(LogBango, Display, TEXT("OnAssetPostImport %s %s --- %s"), *FactoryString, *ObjectString, *GetState(Object));
}

// TODO erase
void UBangoEditorSubsystem::OnPackageDeleted(UPackage* Package) const
{
	//UE_LOG(LogBango, Display, TEXT("OnPackageDeleted: %s"), *Package->GetName());
}

// TODO erase
void UBangoEditorSubsystem::OnAssetsAddExtraObjectsToDelete(TArray<UObject*>& Objects) const
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetsAddExtraObjectsToDelete:"));
	
	for (auto X : Objects)
	{
		//UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

// TODO erase
void UBangoEditorSubsystem::OnAssetsPreDelete(const TArray<UObject*>& Objects) const
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetsPreDelete:"));
	
	for (auto X : Objects)
	{
		//UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

// TODO erase
void UBangoEditorSubsystem::OnAssetsDeleted(const TArray<UClass*>& Classes)
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetsDeleted:"));
	
	for (auto X : Classes)
	{
		//UE_LOG(LogBango, Display, TEXT("     %s --- %s"), *X->GetName(), *GetState(X));
	}
}

// TODO erase??????
void UBangoEditorSubsystem::OnDuplicateActorsBegin()
{
	//UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsBegin"));
	bDuplicateActorsActive = true;
}

// TODO erase??????
void UBangoEditorSubsystem::OnDuplicateActorsEnd()
{
	//UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsEnd"));
	bDuplicateActorsActive = false;
}

// TODO erase
void UBangoEditorSubsystem::OnLevelActorAdded(AActor* Actor) const
{
	//UE_LOG(LogBango, Display, TEXT("OnLevelActorAdded: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
}

// TODO erase
void UBangoEditorSubsystem::OnLevelActorDeleted(AActor* Actor) const
{
	//UE_LOG(LogBango, Display, TEXT("OnLevelActorDeleted: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
	
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(Comps);
	
	for (auto X : Comps)
	{
		//UE_LOG(LogBango, Display, TEXT("Deleted comp... %s"), *(X->GetName()));
	}
}

// TODO document what this is for?
void UBangoEditorSubsystem::OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap)
{
	auto DelayCollectGarbage = FTimerDelegate::CreateLambda([] ()
	{
		CollectGarbage(RF_NoFlags);	
	});
		
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayCollectGarbage);
}

// TODO erase
void UBangoEditorSubsystem::PreSaveWorldWithContext(UWorld* World, FObjectPreSaveContext ObjectPreSaveContext) const
{
	//UE_LOG(LogBango, Display, TEXT("PreSaveWorldWithContext"));
	//ObjectTools::ForceDeleteObjects( { ScriptPackage }, true);
}

// TODO erase
void UBangoEditorSubsystem::OnObjectConstructed(UObject* Object) const
{
	//UE_LOG(LogBango, Display, TEXT("OnObjectConstructed: %s --- %s"), *Object->GetName(), *GetState(Object));
}

void UBangoEditorSubsystem::OnObjectRenamed(UObject* RenamedObject, UObject* RenamedObjectOuter, FName OldName) const
{
	if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(RenamedObject))
	{
		if (!IsValid(RenamedObject) || RenamedObject->HasAnyFlags(RF_ArchetypeObject))
		{
			return;
		}
		
		if (Bango::IsComponentInEditedLevel(ScriptComponent))
		{
			ScriptComponent->OnRename();
			OnScriptGenerated.Broadcast();
		}
	}
	
	//UE_LOG(LogBango, Display, TEXT("OnObjectRenamed: %s  %s  %s --- %s"), *OuterString, *ObjectString, *OldName.ToString(), *GetState(RenamedObjectOuter));
}

// TODO erase
void UBangoEditorSubsystem::OnAssetLoaded(UObject* Object) const
{
	//UE_LOG(LogBango, Display, TEXT("OnAssetLoaded: %s --- %s"), *Object->GetName(), *GetState(Object));
}

// TODO erase
void UBangoEditorSubsystem::OnObjectModified(UObject* Object) const
{
	//UE_LOG(LogBango, Display, TEXT("OnObjectModified: %s --- %s"), *Object->GetName(), *GetState(Object));
}

// TODO erase
void CheckComponentOrigin(UActorComponent* Component)
{
	if (!Component)
	{
		return;
	}
	
	if (Component->IsDefaultSubobject())
	{
		UE_LOG(LogBango, Log, TEXT("Component created via CreateDefaultSubobject in C++"));
	}
	else if (Component->CreationMethod == EComponentCreationMethod::Instance)
	{
		UE_LOG(LogBango, Log, TEXT("Component created via NewObject or added in Level Editor"));
	}
	else if (Component->CreationMethod == EComponentCreationMethod::SimpleConstructionScript)
	{
		UE_LOG(LogBango, Log, TEXT("Component added in Blueprint editor"));
	}
	else if (Component->CreationMethod == EComponentCreationMethod::UserConstructionScript)
	{
		UE_LOG(LogBango, Log, TEXT("Component created in Blueprint Construction Script"));
	}
}

void UBangoEditorSubsystem::OnScriptContainerCreated(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	check(Outer);
	check(ScriptContainer);
	
	if (!IsValid(Outer) || !Outer->IsValidLowLevel() || Outer->HasAnyFlags(RF_Transient))
	{
		return;
	}
	
	FString NewBlueprintName;
	UPackage* ScriptPackage = nullptr;
	UBangoScriptBlueprint* Blueprint = nullptr;
	
	if (ScriptContainer->Guid.IsValid())
	{
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
		// This creation is from a new addition
		Outer->Modify();
		
		ScriptContainer->Guid = FGuid::NewGuid();
		
#if 0
		// This is storing my script blueprints in __BangoScripts__ packages - this is harder to figure out how to manage reliably and is WIP
		// Pros: similar to OFPA, designers don't have VCS conflicts... and you can have multiple with the same name
		ScriptPackage = Bango::Editor::MakePackageForScript(Outer, NewBlueprintName, ScriptContainer->Guid);
		FString BPName = UBangoScriptBlueprint::GetAutomaticName(Outer);
		Blueprint = Bango::Editor::MakeScriptAsset(ScriptPackage, BPName , ScriptContainer->Guid);
#endif
		
#if 1
		// This is storing my script blueprints inside the level .umap file
		AActor* Actor = Outer->GetTypedOuter<AActor>();
		ULevel* Level = Actor->GetLevel();
		FString NominalBPName = UBangoScriptBlueprint::GetAutomaticName(Outer);
		FString BPName = NominalBPName;
		
		int32 Inc = 1;
		
		while (FindObject<UObject>(Level, *BPName) != NULL)
		{
			BPName = FString::Format(TEXT("{0}_{1}"), {NominalBPName, Inc});
		}
		
		Blueprint = Cast<UBangoScriptBlueprint>(FKismetEditorUtilities::CreateBlueprint(UBangoScript::StaticClass(), Level, FName(BPName), BPTYPE_Normal, UBangoScriptBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass()));
#endif
		
		if (Blueprint)
		{
			Blueprint->SetGuid(ScriptContainer->Guid);
		}

		//check(Blueprint);
	}

	if (Blueprint /*&& ScriptPackage*/)
	{
		ScriptContainer->ScriptClass = Blueprint->GeneratedClass;
	
		FAssetRegistryModule::AssetCreated(Blueprint);
		(void)ScriptPackage->MarkPackageDirty();
	
		// Tells FBangoScript property type customizations to regenerate
		OnScriptGenerated.Broadcast();	
	}
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
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->ScriptClass); 
	check(Blueprint);
	
	UPackage* ScriptPackage = Blueprint->GetPackage();
	check(ScriptPackage);
	
	UAssetEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	check(Subsystem);

	Subsystem->CloseAllEditorsForAsset(Blueprint);

	Blueprint->SoftDelete();
	
	ScriptContainer->Unset();
	
	(void)ScriptPackage->MarkPackageDirty();

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
	
	(void)ScriptPackage->MarkPackageDirty();
	
	auto Lambda = FTimerDelegate::CreateLambda([ScriptPackage] ()
	{
		if (ScriptPackage->IsValidLowLevel())
		{
			ScriptPackage->RemoveFromRoot();
			
			//int32 Deleted = ObjectTools::DeleteObjects( { ScriptPackage }, false );
			FEditorFileUtils::FPromptForCheckoutAndSaveParams Params;
			Params.bPromptToSave = false;
		
			EAppReturnType::Type Return = FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("DeleteScriptPackage_ConfirmYesNo", "Delete script package? This will clear your undo history. If you press no, you will be prompted to save the empty script package on level change or shutdown."));
		
			switch (Return)
			{
				case EAppReturnType::Yes:
				{
					//ObjectTools::ForceDeleteObjects( { ScriptPackage }, false);				
					//ObjectTools::CleanupAfterSuccessfulDelete( { ScriptPackage }, true);
					FEditorFileUtils::PromptForCheckoutAndSave( { ScriptPackage }, Params);

					break;
				}
				case EAppReturnType::No:
				{
					break;
				}
				default:
				{
					checkNoEntry();
				}
			}
		}
		
		Bango::Editor::DeleteEmptyScriptFolders(); // TODO this should be removed. I only want this to run on editor shutdown.
	});
	
	if (ScriptPackage != GetTransientPackage() && !Outer->HasAnyFlags(RF_WasLoaded))
	{
		// TODO I can't figure out how to get the stupid asset manager to update IMMEDIATELY. I can't delete the package on the same frame.
		GEditor->GetTimerManager()->SetTimerForNextTick(Lambda);
	}
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
	
	//GEditor->GetTimerManager()->SetTimerForNextTick(DelayOneFrame);
	
	/*
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->ScriptClass);
	
	FString BPName;
	UPackage* PackageForDuplicate = Bango::Editor::MakePackageForScript(Outer, BPName);
	check(PackageForDuplicate);
*/
	
	//FGuid = 
}

void UBangoEditorSubsystem::OnRequestNewID(AActor* Actor) const
{
	FBangoEditorMenus::SetEditActorID(Actor, true);
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
	
	ScriptClass->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty | REN_DontCreateRedirectors | REN_NonTransactional);
	
	ObjectTools::DeleteObjects( { ScriptPackage } );
	
	Bango::Editor::DeleteEmptyFolderFromDisk(Bango::Editor::ScriptRootFolder);
	
	//Blueprint->	ListenForUndelete();
}

UBangoScriptBlueprint* UBangoEditorSubsystem::RetrieveDeletedScript(FGuid Guid)
{
	/*
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
	*/
	
	return nullptr;
}

#undef LOCTEXT_NAMESPACE