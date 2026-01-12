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
	
	FBangoEditorDelegates::OnScriptContainerCreated.AddUObject(this, &ThisClass::OnLevelScriptContainerCreated);
	FBangoEditorDelegates::OnScriptContainerDestroyed.AddUObject(this, &ThisClass::OnLevelScriptContainerDestroyed);
	FBangoEditorDelegates::OnScriptContainerDuplicated.AddUObject(this, &ThisClass::OnLevelScriptContainerDuplicated);
	
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
	if (Bango::Editor::IsComponentInEditedLevel(ScriptComponent))
	{
		// This was undoing a deletion event; we will want to restore a script from the transient package
		
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
	else
	{
		// This was undoing a creation event; we will want to delete the blueprint asset
		
		if (ISourceControlModule::Get().IsEnabled())
		{
			TSoftClassPtr<UBangoScript> ScriptClass = ScriptComponent->ScriptContainer.GetScriptClass();
		
			if (!ScriptClass)
			{
				ScriptClass = ScriptComponent->__UNDO_Script.GetScriptClass();
			}
		
			OnLevelScriptContainerDestroyed(ScriptComponent, ScriptClass);	
		}
		else
		{
			TSoftClassPtr<UBangoScript> ScriptClass = ScriptComponent->ScriptContainer.GetScriptClass();
		
			if (!ScriptClass)
			{
				ScriptClass = ScriptComponent->__UNDO_Script.GetScriptClass();
			}
		
			OnLevelScriptContainerDestroyed(ScriptComponent, ScriptClass);	
		}
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

// TODO erase??????
void UBangoLevelScriptsEditorSubsystem::OnDuplicateActorsBegin()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsBegin"));
	bDuplicateActorsActive = true;
}

// TODO erase??????
void UBangoLevelScriptsEditorSubsystem::OnDuplicateActorsEnd()
{
	UE_LOG(LogBango, Display, TEXT("OnDuplicateActorsEnd"));
	bDuplicateActorsActive = false;
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
	// TODO check if I need to do this. The intent is to obliterate "undo" soft delete script assets.
	auto DelayCollectGarbage = FTimerDelegate::CreateLambda([] ()
	{
		CollectGarbage(RF_NoFlags);	
	});
		
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayCollectGarbage);
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
			Blueprint->Actor = Actor;
			
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
}

// TODO - find path for 1) add new component, 2) save, 3) undo add new component --- it leaves the script .uasset file in place instead of deleting it. Undoing an add is not detected as a destroy?
void UBangoLevelScriptsEditorSubsystem::OnLevelScriptContainerDestroyed(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptClass)
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnLevelScriptContainerDestroyed: %s, %s"), *Outer->GetName(), *ScriptClass.ToSoftObjectPath().GetAssetPathString());
	
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
			NewBlueprint->Actor = Outer->GetTypedOuter<AActor>();	
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
}

void UBangoLevelScriptsEditorSubsystem::OnRequestNewID(AActor* Actor) const
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnRequestNewID: %s"), *Actor->GetName());
	FBangoEditorMenus::SetEditActorID(Actor, true);
}

void UBangoLevelScriptsEditorSubsystem::SoftDeleteLevelScriptPackage(TSoftClassPtr<UBangoScript> ScriptClass)
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("SoftDeleteLevelScriptPackage: %s"), *ScriptClass.ToSoftObjectPath().ToString());
	
	// I have to delay this by one frame or else I get really weird editor crashes, 
	// something about running ObjectTools::ForceDeleteObjects from my component's OnComponentDestroy causes the garbage collector to freak out.
	
	auto DelayedDelete = [ScriptClass] ()
	{
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
		
		UBangoDummyObject* WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt = NewObject<UBangoDummyObject>(OldPackage);
		
		int32 NumDelete = ObjectTools::ForceDeleteObjects( { WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt }, false );
				
		if (NumDelete == 0)
		{
			ObjectTools::ForceDeleteObjects( { WhyDoINeedToPutADummyObjectIntoAPackageToDeleteIt } );
		}

		Bango::Editor::DeleteEmptyLevelScriptFolders();
	};
	
	GEditor->GetTimerManager()->SetTimerForNextTick(DelayedDelete);
}

UBangoScriptBlueprint* UBangoLevelScriptsEditorSubsystem::RetrieveDeletedLevelScript(FGuid Guid)
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("RetrieveDeletedLevelScript: %s"), *Guid.ToString());
	
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