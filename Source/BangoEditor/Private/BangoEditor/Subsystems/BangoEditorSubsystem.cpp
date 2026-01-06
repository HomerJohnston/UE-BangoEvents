#include "BangoEditorSubsystem.h"

#include "AssetToolsModule.h"
#include "ContentBrowserDataSubsystem.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "IContentBrowserDataModule.h"
#include "ISourceControlModule.h"
#include "MeshPaintVisualize.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScript.h"
#include "Bango/Core/BangoScriptContainer.h"
#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/DevTesting/BangoDummyObject.h"
#include "BangoEditor/Menus/BangoEditorMenus.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "BangoEditor/Utilities/BangoFolderUtility.h"
#include "Developer/AssetTools/Private/AssetTools.h"
#include "Editor/Transactor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Helpers/BangoHideScriptFolderFilter.h"
#include "Kismet2/KismetReinstanceUtilities.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "Serialization/FindObjectReferencers.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/PropertyIterator.h"
#include "Widgets/Notifications/SNotificationList.h"

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
	
	// FEditorDelegates::OnAssetPostImport.AddUObject(this, &ThisClass::OnAssetPostImport);
	// FEditorDelegates::OnPackageDeleted.AddUObject(this, &ThisClass::OnPackageDeleted);
	// FEditorDelegates::OnAssetsAddExtraObjectsToDelete.AddUObject(this, &ThisClass::OnAssetsAddExtraObjectsToDelete);
	// FEditorDelegates::OnAssetsPreDelete.AddUObject(this, &ThisClass::OnAssetsPreDelete);
	// FEditorDelegates::OnAssetsDeleted.AddUObject(this, &ThisClass::OnAssetsDeleted);
	FEditorDelegates::OnDuplicateActorsBegin.AddUObject(this, &ThisClass::OnDuplicateActorsBegin);
	FEditorDelegates::OnDuplicateActorsEnd.AddUObject(this, &ThisClass::OnDuplicateActorsEnd);
	// FCoreUObjectDelegates::OnObjectConstructed.AddUObject(this, &ThisClass::OnObjectConstructed);
	// FCoreUObjectDelegates::OnObjectRenamed.AddUObject(this, &ThisClass::OnObjectRenamed);
	// FCoreUObjectDelegates::OnAssetLoaded.AddUObject(this, &ThisClass::OnAssetLoaded);
	// FCoreUObjectDelegates::OnObjectModified.AddUObject(this, &ThisClass::OnObjectModified);
	
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
	// TODO build a setup to register other class types for undo handling
	// TODO dismantle mount everest into functions
	if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(GetValid(Object)))
	{
		if (Bango::IsComponentInEditedLevel(ScriptComponent))
		{
			// Find a script container on this thing and make it do stuff
			for (TFieldIterator<FProperty> It(UBangoScriptComponent::StaticClass()); It; ++It)
			{
				const FProperty* Property = *It;
				
				if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
					if (StructProperty->Struct == FBangoScriptContainer::StaticStruct())
					{
						void* Value = nullptr;
						Value = StructProperty->ContainerPtrToValuePtr<void>(Object);
						
						if (Value)
						{
							FBangoScriptContainer* ScriptContainer = reinterpret_cast<FBangoScriptContainer*>(Value);
							
							TArray<UObject*> TransientObjects;
							GetObjectsWithOuter(GetTransientPackage(), TransientObjects);
	
							for (UObject* TransientObject : TransientObjects)
							{
								if (UBangoScriptBlueprint* ScriptBlueprint = Cast<UBangoScriptBlueprint>(TransientObject))
								{
									if (ScriptBlueprint->ScriptGuid == ScriptContainer->GetGuid())
									{
										if (FPackageName::DoesPackageExist(ScriptBlueprint->DeletedPackagePath.GetLongPackageName()))
										{
											UE_LOG(LogBango, Warning, TEXT("Tried to restore deleted Bango Blueprint but there was another package at the location. Invalidating this Script Container property."));
											ScriptContainer->Unset();
											return;
										}
									
										if (ISourceControlModule::Get().IsEnabled())
										{
											/*
											ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();

											TArray<UPackage*> PackagesToSave;
											for (UPackage* Package : DirtyDatabasePackages)
											{
												FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(Package, EStateCacheUsage::Use);
												if (SourceControlState->IsCheckedOutOther())
												{
													UE_LOG(LogAnimationCompression, Warning, TEXT("Package %s is already checked out by someone, will not check out"), *SourceControlState->GetFilename());
												}
												else if (!SourceControlState->IsCurrent())
												{
													UE_LOG(LogAnimationCompression, Warning, TEXT("Package %s is not at head, will not check out"), *SourceControlState->GetFilename());
												}
												else if (SourceControlState->CanCheckout())
												{
													const ECommandResult::Type StatusResult = SourceControlProvider.Execute(ISourceControlOperation::Create<FCheckOut>(), Package);
													if (StatusResult != ECommandResult::Succeeded)
													{
														UE_LOG(LogAnimationCompression, Log, TEXT("Package %s failed to check out"), *SourceControlState->GetFilename());
														bFailedToSave = true;
													}
													else
													{
														PackagesToSave.Add(Package);
													}
												}
												else if (!SourceControlState->IsSourceControlled() || SourceControlState->CanEdit())
												{
													PackagesToSave.Add(Package);
												}
											}

											UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);
											ISourceControlModule::Get().QueueStatusUpdate(PackagesToSave);
											*/
										}
										else
										{
											// No source control, just restore the transient copy we made on destroy into a new package. NOTE: The new package will have a few bytes different.
											UPackage* RestoredPackage = CreatePackage(*ScriptBlueprint->DeletedPackagePath.ToString());
											RestoredPackage->SetFlags(RF_Public);
											RestoredPackage->SetPackageFlags(PKG_NewlyCreated);
											RestoredPackage->SetPersistentGuid(ScriptBlueprint->DeletedPackagePersistentGuid);
											RestoredPackage->SetPackageId(ScriptBlueprint->DeletedPackageId);
										
											ScriptBlueprint->Rename(*ScriptBlueprint->DeletedName, RestoredPackage, REN_DontCreateRedirectors | REN_DoNotDirty | REN_NonTransactional);
											ScriptBlueprint->Modify();
											ScriptBlueprint->ClearFlags(RF_Transient);
										
											FAssetRegistryModule::AssetCreated(ScriptBlueprint);
											(void)ScriptBlueprint->MarkPackageDirty();
											//										RestoredPackage->FullyLoad();
										
											GEditor->GetEditorSubsystem<UEditorAssetSubsystem>()->SaveLoadedAsset(ScriptBlueprint, false);
										}
									}
								}
							}
						}
					}
				}
			}
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
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsBegin"));
	bDuplicateActorsActive = true;
}

// TODO erase??????
void UBangoEditorSubsystem::OnDuplicateActorsEnd()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsEnd"));
	bDuplicateActorsActive = false;
}

// TODO erase
void UBangoEditorSubsystem::OnLevelActorAdded(AActor* Actor) const
{
	UE_LOG(LogBango, Display, TEXT("OnLevelActorAdded: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
}

// TODO erase
void UBangoEditorSubsystem::OnLevelActorDeleted(AActor* Actor) const
{
	UE_LOG(LogBango, Display, TEXT("OnLevelActorDeleted: %s --- %s"), *Actor->GetActorLabel(), *GetState(Actor));
	
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

void UBangoEditorSubsystem::OnScriptContainerCreated(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name, bool bImmediate)
{
	if (bDuplicateActorsActive)
	{
		//OnScriptContainerDuplicated(Outer, ScriptContainer);
		return;
	}
	
	TWeakObjectPtr<UBangoEditorSubsystem> WeakThis = this;
	TWeakObjectPtr<UObject> WeakOuter = Outer;
	
	auto Delayed = FTimerDelegate::CreateLambda([WeakThis, WeakOuter, ScriptContainer, Name] ()
	{
		if (!WeakThis.IsValid() || !WeakOuter.IsValid())
		{
			return;
		}
		
		UBangoEditorSubsystem* This = WeakThis.Get();
		UObject* Outer = WeakOuter.Get();
		
		FString NewBlueprintName = Name;
		UPackage* ScriptPackage = nullptr;
		UBangoScriptBlueprint* Blueprint = nullptr;
		
		if (ScriptContainer->GetGuid().IsValid())
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
			
			ScriptContainer->GenerateGuid();
			
	#if 1
			// This is storing my script blueprints in __BangoScripts__ packages - this is harder to figure out how to manage reliably and is WIP
			// Pros: similar to OFPA, designers don't have VCS conflicts... and you can have multiple with the same name
			AActor* Actor = Outer->GetTypedOuter<AActor>();
			ScriptPackage = Bango::Editor::MakeLevelScriptPackage(Outer, NewBlueprintName, ScriptContainer->GetGuid());
			
			FString ShortPackageName = FPackageName::GetShortName(ScriptPackage);

			Blueprint = Bango::Editor::MakeScriptAsset(ScriptPackage, ShortPackageName, ScriptContainer->GetGuid());
			
			Blueprint->Modify();
			Blueprint->Actor = Actor;
	#endif
			
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
			}
		}

		if (Blueprint)
		{
			ScriptContainer->SetScriptClass(Blueprint->GeneratedClass);
		
			FAssetRegistryModule::AssetCreated(Blueprint);
			(void)ScriptPackage->MarkPackageDirty();
		
			// Tells FBangoScript property type customizations to regenerate
			This->OnScriptGenerated.Broadcast();	
		}
	});
	
	if (!bImmediate)
	{
		GEditor->GetTimerManager()->SetTimerForNextTick(Delayed);
	}
	else
	{
		Delayed.Execute();
	}
}

#pragma region FBlueprintUnloader
struct FBlueprintUnloader
{
public:
	FBlueprintUnloader(UBlueprint* OldBlueprint);

	/** 
	 * Unloads the specified Blueprint (marking it pending-kill, and removing it 
	 * from its outer package). Optionally, will unload the package as well.
	 *
	 * @param  bResetPackage	Whether or not this should unload the entire package.
	 */
	void UnloadBlueprint(const bool bResetPackage);
	
	/** 
	 * Replaces all old references to the original blueprints (its class/CDO/etc.)
	 * @param  NewBlueprint	The blueprint to replace old references with
	 */
	void ReplaceStaleRefs(UBlueprint* NewBlueprint);

private:
	TWeakObjectPtr<UBlueprint> OldBlueprint;
	UClass*  OldGeneratedClass;
	UObject* OldCDO;
	UClass*  OldSkeletonClass;
	UObject* OldSkelCDO;
};


FBlueprintUnloader::FBlueprintUnloader(UBlueprint* OldBlueprintIn)
	: OldBlueprint(OldBlueprintIn)
	, OldGeneratedClass(OldBlueprint->GeneratedClass)
	, OldCDO(nullptr)
	, OldSkeletonClass(OldBlueprint->SkeletonGeneratedClass)
	, OldSkelCDO(nullptr)
{
	if (OldGeneratedClass != nullptr)
	{
		OldCDO = OldGeneratedClass->GetDefaultObject(/*bCreateIfNeeded =*/false);
	}
	if (OldSkeletonClass != nullptr)
	{
		OldSkelCDO = OldSkeletonClass->GetDefaultObject(/*bCreateIfNeeded =*/false);
	}
	OldBlueprint = OldBlueprintIn;
}

void FBlueprintUnloader::UnloadBlueprint(const bool bResetPackage)
{
	if (OldBlueprint.IsValid())
	{
		UBlueprint* UnloadingBp = OldBlueprint.Get();

		UPackage* const OldPackage = UnloadingBp->GetOutermost();
		bool const bIsDirty = OldPackage->IsDirty();

		UPackage* const TransientPackage = GetTransientPackage();
		check(OldPackage != TransientPackage); // is the blueprint already unloaded?
		
		FName const BlueprintName = UnloadingBp->GetFName();
		// move the blueprint to the transient package (to be picked up by garbage collection later)
		FName UnloadedName = MakeUniqueObjectName(TransientPackage, UBlueprint::StaticClass(), BlueprintName);
		UnloadingBp->Rename(*UnloadedName.ToString(), TransientPackage, REN_DontCreateRedirectors | REN_DoNotDirty);
		// @TODO: currently, REN_DoNotDirty does not guarantee that the package 
		//        will not be marked dirty
		OldPackage->SetDirtyFlag(bIsDirty);

		// make sure the blueprint is properly trashed (remove it from the package)
		UnloadingBp->SetFlags(RF_Transient);
		UnloadingBp->ClearFlags(RF_Standalone | RF_Transactional);
		UnloadingBp->RemoveFromRoot();
		UnloadingBp->MarkAsGarbage();
		// if it's in the undo buffer, then we have to clear that...
		if (FKismetEditorUtilities::IsReferencedByUndoBuffer(UnloadingBp))
		{
			GEditor->Trans->Reset(LOCTEXT("UnloadedBlueprint", "Unloaded Blueprint"));
		}

		if (bResetPackage)
		{
			TArray<UPackage*> PackagesToUnload;
			PackagesToUnload.Add(OldPackage);

			FText PackageUnloadError;
			UPackageTools::UnloadPackages(PackagesToUnload, PackageUnloadError);

			if (!PackageUnloadError.IsEmpty())
			{
				const FText ErrorMessage = FText::Format(LOCTEXT("UnloadBpPackageError", "Failed to unload Bluprint '{0}': {1}"),
					FText::FromName(BlueprintName), PackageUnloadError);
				FSlateNotificationManager::Get().AddNotification(FNotificationInfo(ErrorMessage));

				// fallback to manually setting up the package so it can reload 
				// the blueprint 
				ResetLoaders(OldPackage);
				OldPackage->ClearFlags(RF_WasLoaded);
				OldPackage->bHasBeenFullyLoaded = false;
				OldPackage->GetMetaData().RemoveMetaDataOutsidePackage(OldPackage);
			}
		}

		UnloadingBp->ClearEditorReferences();

		// handled in FBlueprintEditor (from the OnBlueprintUnloaded event)
// 		IAssetEditorInstance* EditorInst = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(UnloadingBp, /*bFocusIfOpen =*/false);
// 		if (EditorInst != nullptr)
// 		{
// 			EditorInst->CloseWindow();
// 		}
	}
}

void FBlueprintUnloader::ReplaceStaleRefs(UBlueprint* NewBlueprint)
{
	//--------------------------------------
	// Construct redirects
	//--------------------------------------

	TMap<UObject*, UObject*> Redirects;
	TArray<UObject*> OldObjsNeedingReplacing;

	if (OldBlueprint.IsValid(/*bEvenIfPendingKill =*/true))
	{
		UBlueprint* ToBeReplaced = OldBlueprint.Get(/*bEvenIfPendingKill =*/true);
		if (OldGeneratedClass != nullptr)
		{
			OldObjsNeedingReplacing.Add(OldGeneratedClass);
			Redirects.Add(OldGeneratedClass, NewBlueprint->GeneratedClass);
		}
		if (OldCDO != nullptr)
		{
			OldObjsNeedingReplacing.Add(OldCDO);
			Redirects.Add(OldCDO, NewBlueprint->GeneratedClass->GetDefaultObject());
		}
		if (OldSkeletonClass != nullptr)
		{
			OldObjsNeedingReplacing.Add(OldSkeletonClass);
			Redirects.Add(OldSkeletonClass, NewBlueprint->SkeletonGeneratedClass);
		}
		if (OldSkelCDO != nullptr)
		{
			OldObjsNeedingReplacing.Add(OldSkelCDO);
			Redirects.Add(OldSkelCDO, NewBlueprint->SkeletonGeneratedClass->GetDefaultObject());
		}

		OldObjsNeedingReplacing.Add(ToBeReplaced);
		Redirects.Add(ToBeReplaced, NewBlueprint);

		// clear the object being debugged; otherwise ReplaceInstancesOfClass()  
		// trys to reset it with a new level instance, and OldBlueprint won't 
		// match the new instance's type (it's now a NewBlueprint)
		ToBeReplaced->SetObjectBeingDebugged(nullptr);
	}

	//--------------------------------------
	// Replace old references
	//--------------------------------------

	TArray<UObject*> Referencers;
	// find all objects, still referencing the old blueprint/class/cdo/etc.
	for (auto Referencer : TFindObjectReferencers<UObject>(OldObjsNeedingReplacing, /*PackageToCheck =*/nullptr, /*bIgnoreTemplates =*/false))
	{
		Referencers.Add(Referencer.Value);
	}

	FBlueprintCompileReinstancer::ReplaceInstancesOfClass(OldGeneratedClass, NewBlueprint->GeneratedClass, FReplaceInstancesOfClassParameters());

	for (UObject* Referencer : Referencers)
	{
		FArchiveReplaceObjectRef<UObject>(Referencer, Redirects);
	}
}
#pragma endregion 

void UBangoEditorSubsystem::OnScriptContainerDestroyed(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptClass)
{
	const FSoftObjectPath& ScriptClassPath = ScriptClass.ToSoftObjectPath();

	if (!FPackageName::DoesPackageExist(ScriptClassPath.GetLongPackageName()))
	{
		UE_LOG(LogBango, Warning, TEXT("OnScriptContainerDestroyed called with invalid ScriptClass path (check earlier logs, maybe the .uasset file was deleted already)!"));
		return;	
	}
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptClass); 
	check(Blueprint);

	Outer->Modify();
	
	UPackage* OldPackage = Blueprint->GetPackage();
	
	Blueprint->ClearEditorReferences();

	// TODO make sure i don't leave any raw pointer delay lambda crash dumbassery in my code, i've rewritten this thing a million different ways 
	// We will delay our action by one frame to allow other things to prepare for this. The property type customization needs time to clean itself up.
	auto Fuck = FTimerDelegate::CreateLambda([OldPackage, Blueprint] ()
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(Blueprint);

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

		Bango::Editor::DeleteEmptyScriptFolders(); // TODO maybe this should be removed. I might only want this to run on editor shutdown or a manual run.
	});

	GEditor->GetTimerManager()->SetTimerForNextTick(Fuck);
}

#if 0
void /*UBangoEditorSubsystem::*/OnScriptContainerDestroyed(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	if (!IsExistingScriptContainerValid(Outer, ScriptContainer))
	{
		return;
	}
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->GetScriptClass()); 
	check(Blueprint);
	
	UPackage* ScriptPackage = Blueprint->GetPackage();
	check(ScriptPackage);
	
	UAssetEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	check(Subsystem);

	Subsystem->CloseAllEditorsForAsset(Blueprint);

 	// ******************************************
	
	// Verify the source object
	if (Blueprint)
	{
		UPackage* Package = GetTransientPackage();
		
		ObjectTools::FPackageGroupName PGN;
		PGN.PackageName = Package->GetName();
		PGN.GroupName = TEXT("");
		PGN.ObjectName = Blueprint->ScriptGuid.ToString();

		TSet<UPackage*> ObjectsUserRefusedToFullyLoad;
		
		TArray<UObject*> InternalObjects;
		GetObjectsWithOuter(Blueprint, InternalObjects);
		
		bool bPromptToOverwrite = false;
		
		IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		
		//UObject* NewObject = AssetTools->DuplicateAsset(Blueprint->ScriptGuid.ToString(), GetTransientPackage()->GetPathName(), Blueprint);
		
		//UObject* NewObject = ObjectTools::DuplicateSingleObject(Blueprint, PGN, ObjectsUserRefusedToFullyLoad, bPromptToOverwrite);
		if(NewObject != nullptr)
		{
			// Assets must have RF_Public and RF_Standalone
			const bool bIsAsset = NewObject->IsAsset();
			NewObject->SetFlags(RF_Public | RF_Standalone);

			if (!bIsAsset && NewObject->IsAsset())
			{
				// Notify the asset registry
				// We won't do this because we don't actually want to save this as an asset, it's a temp copy for undo support
				//FAssetRegistryModule::AssetCreated(NewObject);
			}

			/*
			if ( ISourceControlModule::Get().IsEnabled() )
			{
				// Save package here if SCC is enabled because the user can use SCC to revert a change
				TArray<UPackage*> OutermostPackagesToSave;
				OutermostPackagesToSave.Add(NewObject->GetOutermost());

				const bool bCheckDirty = false;
				const bool bPromptToSave = false;
				FEditorFileUtils::PromptForCheckoutAndSave(OutermostPackagesToSave, bCheckDirty, bPromptToSave);

				// now attempt to branch, we can do this now as we should have a file on disk
				SourceControlHelpers::CopyPackage(NewObject->GetOutermost(), OriginalObject->GetOutermost());
			}
			*/
			// analytics create record
			//UAssetToolsImpl::OnNewCreateRecord(NewObject->GetClass(), true);
		}
	}

	
	/*
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
							
	FString TargetName;
	FString TargetPackageName;
	IAssetTools::Get().CreateUniqueAssetName(SelectedAssetObject->GetOutermost()->GetName(), TEXT("_Copy"), TargetPackageName, TargetName);

	// Duplicate the asset.
	UObject* NewAsset = AssetTools.DuplicateAsset(TargetName, FPackageName::GetLongPackagePath(TargetPackageName), SelectedAssetObject.Get());
	*/
	
	
	// ******************************************
	
	Blueprint->ClearEditorReferences();
		
	ScriptContainer->Unset();
	
//#if 1
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
	
	AActor* Actor = Outer->GetTypedOuter<AActor>();
	
	if (Actor)
	{
		TSet<UPackage*> PackagesToSave { Actor->GetPackage() };

		if (PackagesToSave.Num())
		{
			FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
			SaveParams.bCheckDirty = false;
			SaveParams.bPromptToSave = false;
			SaveParams.bIsExplicitSave = true;

			FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave.Array(), SaveParams);
		}
	}
	
	(void)ScriptPackage->MarkPackageDirty();
//#endif
	
	auto Lambda = FTimerDelegate::CreateLambda([Blueprint, ScriptPackage, ScriptContainer] ()
	{		
		if (ScriptPackage->IsValidLowLevel())
		{
			ScriptPackage->RemoveFromRoot();
			
			//int32 Deleted = ObjectTools::DeleteObjects( { Blueprint }, false );
			//FEditorFileUtils::FPromptForCheckoutAndSaveParams Params;
			//Params.bPromptToSave = false;
		
			/*
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
			*/
		}
		
		Bango::Editor::DeleteEmptyScriptFolders(); // TODO maybe this should be removed. I might only want this to run on editor shutdown or a manual run.
	});
	
	//if (ScriptPackage != GetTransientPackage() && !Outer->HasAnyFlags(RF_WasLoaded))
	//{
		// TODO I can't figure out how to get the stupid asset manager to update IMMEDIATELY. I can't delete the package on the same frame.
		GEditor->GetTimerManager()->SetTimerForNextTick(Lambda);
	//}
}
#endif

bool UBangoEditorSubsystem::IsExistingScriptContainerValid(UObject* Outer, FBangoScriptContainer* ScriptContainer)
{
	check(IsValid(Outer));
	check(ScriptContainer);
	
	if (!ScriptContainer->GetScriptClass() || !ScriptContainer->GetGuid().IsValid())
	{
		return false;
	}
	
	return true;
}

void UBangoEditorSubsystem::OnScriptContainerDuplicated(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name)
{
	// When an actor is added to the world containing a CDO ScriptComponent, this function might be called. 
	TWeakObjectPtr<UObject> WeakOuter = Outer;
	TWeakObjectPtr<UBangoEditorSubsystem> WeakThis = this;
	
	auto DelayOneFrame = [WeakThis, WeakOuter, ScriptContainer, Name] ()
	{
		if (!WeakThis.IsValid() || !WeakOuter.IsValid())
		{
			return;
		}
				
		UObject* Outer = WeakOuter.Get();
		UBangoEditorSubsystem* This = WeakThis.Get();
		
		if (!ScriptContainer->GetScriptClass())
		{
			// Switch over to the created path; this path happens when you drag a content blueprint into the scene as it "duplicates" the content asset
			This->OnScriptContainerCreated(Outer, ScriptContainer, Name, true);
			return;
		}
		
		// Stash the referenced blueprint
		UBangoScriptBlueprint* OldBlueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptContainer->GetScriptClass());
		
		// Set up this new duplicated script container
		ScriptContainer->Unset();
		ScriptContainer->GenerateGuid();		
		
		// Dupe the blueprint
		FString BPName = Name;
		UPackage* NewScriptPackage = Bango::Editor::MakeLevelScriptPackage(Outer, BPName, ScriptContainer->GetGuid());
	
		if (!NewScriptPackage)
		{
			UE_LOG(LogBango, Error, TEXT("Tried to create a new script but could not create a package!"));
			return;
		}
		
		UBangoScriptBlueprint* Blueprint = nullptr;
			
		if (OldBlueprint)
		{
			Blueprint = DuplicateObject(OldBlueprint, NewScriptPackage, FName(FPackageName::GetShortName(NewScriptPackage)));
			//FString BPName = UBangoScriptBlueprint::GetAutomaticName(Outer);
			//Blueprint->Rename(*BPName, nullptr, REN_NonTransactional | REN_DontCreateRedirectors);
			Blueprint->Actor = Outer->GetTypedOuter<AActor>();	
		}

		if (Blueprint)
		{
			ScriptContainer->SetScriptClass(Blueprint->GeneratedClass);
	
			FAssetRegistryModule::AssetCreated(Blueprint);
			(void)NewScriptPackage->MarkPackageDirty();
	
			// Tells FBangoScript property type customizations to regenerate
			This->OnScriptGenerated.Broadcast();	
		}
	};
	
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayOneFrame);
}

void UBangoEditorSubsystem::OnRequestNewID(AActor* Actor) const
{
	FBangoEditorMenus::SetEditActorID(Actor, true);
}

void UBangoEditorSubsystem::SoftDeleteScriptPackage(TSoftClassPtr<UBangoScript> ScriptClass)
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
	TArray<UObject*> AllTransientObjects;
	GetObjectsWithOuter(GetTransientPackage(), AllTransientObjects);
	
	int32 Index = AllTransientObjects.IndexOfByPredicate([Guid] (const UObject* Element)
	{
		if (const UBangoScriptBlueprint* Script = Cast<UBangoScriptBlueprint>(Element))
		{
			return Script->ScriptGuid == Guid;
		}
		
		return false;
	});
	
	if (Index != INDEX_NONE)
	{
		return Cast<UBangoScriptBlueprint>(AllTransientObjects[Index]);
	}
	
	return nullptr;
}

#undef LOCTEXT_NAMESPACE