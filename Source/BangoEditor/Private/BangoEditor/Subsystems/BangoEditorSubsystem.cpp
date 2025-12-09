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
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "Helpers/BangoHideScriptFolderFilter.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"

TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> UBangoEditorSubsystem::Filter;

void UBangoEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UContentBrowserDataSubsystem>();
	//Collection.InitializeDependency<UBangoScriptHelperSubsystem>();

	Filter = MakeShared<FBangoHideScriptFolderFilter>();
	UContentBrowserDataSubsystem* ContentBrowserData = IContentBrowserDataModule::Get().GetSubsystem();
	//ContentBrowserData->RegisterCreateHideFolderIfEmptyFilter(FContentBrowserCreateHideFolderIfEmptyFilter::CreateLambda([this] () { return Filter; }));

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
	
	//GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentCreated.AddUObject(this, &ThisClass::OnScriptComponentCreated);
	//GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentDestroyed.AddUObject(this, &ThisClass::OnScriptComponentDestroyed);
	FBangoEditorDelegates::OnScriptComponentCreated.AddUObject(this, &ThisClass::OnScriptComponentCreated);
	FBangoEditorDelegates::OnScriptComponentDestroyed.AddUObject(this, &ThisClass::OnScriptComponentDestroyed);
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

void UBangoEditorSubsystem::OnObjectRenamed(UObject* Outer, UObject* Object, FName Name) const
{
	FString OuterString = Outer ? Outer->GetName() : "No Outer";
	
	FString ObjectString = Object ? Object->GetName() : "No Object";

	UE_LOG(LogBango, Display, TEXT("OnObjectRenamed: %s  %s  %s --- %s"), *OuterString, *ObjectString, *Name.ToString(), *GetState(Object));
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

void UBangoEditorSubsystem::OnScriptComponentCreated(UBangoScriptComponent* BangoScriptComponent, UBangoScriptBlueprint* ExistingBlueprint) const
{
	UPackage* ActorPackage = BangoScriptComponent->GetPackage();

	/**/
	//Bango::Editor::CreateBlueprint("/Game/TESTTESTTEST", UBangoScript::StaticClass());
	/**/
	
	FString BPName;
	UPackage* ScriptPackage = Bango::Editor::MakeScriptPackage(BangoScriptComponent, ActorPackage, BPName);
	
	if (!ScriptPackage)
	{
		return;
	}
	
	FGuid Guid = BangoScriptComponent->GetScriptGuid();

	UBangoScriptBlueprint* Blueprint = ExistingBlueprint;
	
	if (Blueprint)
	{
		Blueprint->Rename(nullptr, ScriptPackage, REN_DontCreateRedirectors | REN_NonTransactional);
	}
	else
	{
		Blueprint = Bango::Editor::MakeScriptAsset(ScriptPackage, BPName, Guid);
	}

	BangoScriptComponent->SetScriptBlueprint(Blueprint);
	Blueprint->Modify();
	Blueprint->GeneratedClass->Modify();
	ScriptPackage->SetAssetAccessSpecifier(EAssetAccessSpecifier::Public);
	FKismetEditorUtilities::CompileBlueprint(Blueprint);
	
	FAssetRegistryModule::AssetCreated(Blueprint);
	ScriptPackage->MarkPackageDirty();
	Blueprint->SetGuid(BangoScriptComponent->GetScriptGuid());

	
	UScriptStruct* ScriptContainerType = FBangoScriptContainer::StaticStruct();

	FPropertyChangedEvent DummyEvent1(UBangoScriptComponent::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UBangoScriptComponent, Script)));
	FPropertyChangedEvent DummyEvent3(ScriptContainerType->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass)));
	BangoScriptComponent->PostEditChangeProperty(DummyEvent1);
	BangoScriptComponent->PostEditChangeProperty(DummyEvent3);
	OnScriptGenerated.Broadcast();
}

void UBangoEditorSubsystem::OnScriptComponentDestroyed(UBangoScriptComponent* BangoScriptComponent)
{
	TWeakObjectPtr<UBangoScriptBlueprint> Blueprint = BangoScriptComponent->GetScriptBlueprint();
	
	if (!Blueprint.IsValid())
	{
		return;
	}
	
	UPackage* ScriptPackage = Blueprint->GetPackage();
	
	if (!ScriptPackage)
	{
		return;
	}
	
	TPair<FGuid, TStrongObjectPtr<UBangoScriptBlueprint>> SoftDeletedScript = { BangoScriptComponent->GetScriptGuid(), Blueprint.Pin() };
	
	SoftDeletedScripts.Add( SoftDeletedScript );
	
	Blueprint->Modify();
	Blueprint->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
	Blueprint->ListenForUndelete(BangoScriptComponent->GetScriptGuid());
	
	BangoScriptComponent->UnsetScript();
	
	//UPackage* ActorPackage = BangoScriptComponent->GetPackage();
	//FString PackageFileName = FPackageName::LongPackageNameToFilename(ActorPackage->GetName(), FPackageName::GetAssetPackageExtension());
	//UPackage::SavePackage(ActorPackage, BangoScriptComponent->GetOwner(), *PackageFileName, FSavePackageArgs() );
	
	//FString ScriptPackageFilename = FPackageName::LongPackageNameToFilename(ScriptPackage->GetName(), FPackageName::GetAssetPackageExtension());
	//UPackage::SavePackage(ScriptPackage, nullptr, *ScriptPackageFilename, FSavePackageArgs());
	
	Blueprint->ClearEditorReferences();
	
	ResetLoaders(ScriptPackage);
	CollectGarbage(RF_NoFlags);
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	FString OldScriptPackagePath = FPackageName::GetLongPackagePath(ScriptPackage->GetPathName());

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
	
	//CollectGarbage(RF_NoFlags);
	
	// TODO I can't figure out how to get the stupid asset manager to update IMMEDIATELY
	GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateLambda([ScriptPackage] ()
	{
		int32 Deleted = ObjectTools::DeleteObjects( {ScriptPackage}, false );
		UE_LOG(LogBango, Display, TEXT("Deleted %i script packages"), Deleted);
	}));
}

void UBangoEditorSubsystem::SoftDeleteScriptPackage(TSubclassOf<UBangoScript> ScriptClass)
{
	UPackage* ScriptPackage = ScriptClass->GetOuterUPackage();
	UBangoScriptBlueprint* Blueprint = Cast<UBangoScriptBlueprint>(UBlueprint::GetBlueprintFromClass(ScriptClass));
	
	if (!ScriptPackage || !Blueprint)
	{
		return;
	}
	
	TPair<FGuid, TStrongObjectPtr<UBangoScriptBlueprint>> SoftDeletedScript = { ScriptClass->GetDefaultObject<UBangoScript>()->GetScriptGuid(), TStrongObjectPtr<UBangoScriptBlueprint>(Blueprint) };
	SoftDeletedScripts.Add(SoftDeletedScript);
	
	ScriptClass->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty | REN_DontCreateRedirectors);
	
	ObjectTools::DeleteObjects( { ScriptPackage } );
	
	Bango::Editor::DeleteEmptyFolderFromDisk(Bango::Editor::ScriptRootFolder);
	
	//Blueprint->	ListenForUndelete();
}

UBangoScriptBlueprint* UBangoEditorSubsystem::RetrieveSoftDeletedScript(FGuid Guid)
{
	int32 Index = SoftDeletedScripts.IndexOfByPredicate( [Guid] (const TPair<FGuid, TStrongObjectPtr<UBangoScriptBlueprint>>& Element)
	{
		return Element.Key == Guid;
	});
	
	if (Index >= 0)
	{
		TStrongObjectPtr<UBangoScriptBlueprint> Popped = SoftDeletedScripts[Index].Value;
		
		SoftDeletedScripts.RemoveAt(Index);
		
		return Popped.Get();
	}
	
	return nullptr;
}
