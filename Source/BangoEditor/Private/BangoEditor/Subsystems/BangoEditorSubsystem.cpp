#include "BangoEditorSubsystem.h"

#include "ContentBrowserDataSubsystem.h"
#include "IContentBrowserDataModule.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "Helpers/BangoHideScriptFolderFilter.h"
#include "UObject/SavePackage.h"

TSharedPtr<IContentBrowserHideFolderIfEmptyFilter> UBangoEditorSubsystem::Filter;

void UBangoEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UContentBrowserDataSubsystem>();
	//Collection.InitializeDependency<UBangoScriptHelperSubsystem>();

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

void UBangoEditorSubsystem::OnScriptComponentCreated(UBangoScriptComponent* BangoScriptComponent) const
{
	UPackage* ActorPackage = BangoScriptComponent->GetPackage();

	FString BPName;
	UPackage* ScriptPackage = Bango::Editor::MakeScriptPackage(BangoScriptComponent, ActorPackage, BPName);
	
	if (!ScriptPackage)
	{
		return;
	}
	
	FGuid Guid = BangoScriptComponent->GetScriptGuid();

	UBlueprint* Blueprint = Bango::Editor::MakeScriptAsset(ScriptPackage, BPName, Guid);
	
	if (!Blueprint)
	{
		return;
	}

	//if (Bango::Editor::SaveScriptPackage(ScriptPackage, Blueprint))
	//{
		BangoScriptComponent->SetScriptBlueprint(Blueprint);
	//}
	UScriptStruct* ScriptContainerType = FBangoScriptContainer::StaticStruct();
	
	//FProperty* Prop=
	
	FPropertyChangedEvent DummyEvent1(UBangoScriptComponent::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UBangoScriptComponent, Script)));
	FPropertyChangedEvent DummyEvent2(ScriptContainerType->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptBlueprint)));
	FPropertyChangedEvent DummyEvent3(ScriptContainerType->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass)));
	BangoScriptComponent->PostEditChangeProperty(DummyEvent1);
	BangoScriptComponent->PostEditChangeProperty(DummyEvent2);
	BangoScriptComponent->PostEditChangeProperty(DummyEvent3);
	
	OnScriptGenerated.Broadcast();
}

void UBangoEditorSubsystem::OnScriptComponentDestroyed(UBangoScriptComponent* BangoScriptComponent)
{
	UBlueprint* Blueprint = BangoScriptComponent->GetScriptBlueprint();
	
	if (!Blueprint)
	{
		return;
	}
	
	UPackage* ScriptPackage = Blueprint->GetPackage();
	
	if (!ScriptPackage)
	{
		return;
	}
	
	TPair<FGuid, TStrongObjectPtr<UBlueprint>> SoftDeletedScript = { BangoScriptComponent->GetScriptGuid(), TStrongObjectPtr<UBlueprint>(Blueprint) };
	
	SoftDeletedScripts.Add( SoftDeletedScript );
	
	Blueprint->Modify();
	Blueprint->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
	
	BangoScriptComponent->UnsetScript();
	
	UPackage* ActorPackage = BangoScriptComponent->GetPackage();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(ActorPackage->GetName(), FPackageName::GetAssetPackageExtension());
	UPackage::SavePackage(ActorPackage, BangoScriptComponent->GetOwner(), *PackageFileName, FSavePackageArgs() );
	
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
	
	// TODO I can't figure out how to get the stupid asset manager to update IMMEDIATELY - 
	GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateLambda([ScriptPackage] ()
	{
		int32 Deleted = ObjectTools::DeleteObjects( {ScriptPackage}, false );
		UE_LOG(LogBango, Display, TEXT("Deleted %i script packages"), Deleted);
		
		DeleteEmptyFolders(Bango::Editor::GetScriptRootContentFolder());
		//Bango::Editor::DeleteEmptyFolderFromDisk(FBangoPackageHelper::GetBangoScriptsFolderName());
	}));
}

void UBangoEditorSubsystem::SoftDeleteScriptPackage(TSubclassOf<UBangoScriptInstance> ScriptClass)
{
	UPackage* ScriptPackage = ScriptClass->GetOuterUPackage();
	UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(ScriptClass);
	
	if (!ScriptPackage || !Blueprint)
	{
		return;
	}
	
	TPair<FGuid, TStrongObjectPtr<UBlueprint>> SoftDeletedScript = { ScriptClass->GetDefaultObject<UBangoScriptInstance>()->GetScriptGuid(), TStrongObjectPtr<UBlueprint>(Blueprint) };
	SoftDeletedScripts.Add(SoftDeletedScript);
	
	ScriptClass->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty | REN_DontCreateRedirectors);
	
	ObjectTools::DeleteObjects( { ScriptPackage } );
	
	Bango::Editor::DeleteEmptyFolderFromDisk(Bango::Editor::ScriptRootFolder);
}

UBlueprint* UBangoEditorSubsystem::RetrieveSoftDeletedScript(FGuid Guid)
{
	int32 Index = SoftDeletedScripts.IndexOfByPredicate( [Guid] (const TPair<FGuid, TStrongObjectPtr<UBlueprint>>& Element)
	{
		return Element.Key == Guid;
	});
	
	if (Index >= 0)
	{
		TStrongObjectPtr<UBlueprint> Popped = SoftDeletedScripts[Index].Value;
		
		SoftDeletedScripts.RemoveAt(Index);
		
		return Popped.Get();
	}
	
	return nullptr;
}

void UBangoEditorSubsystem::DeleteEmptyFolders(const FString& RootPath, bool bShowSlowTask)
{
	TArray<FString> FoldersEmpty;
	GetEmptyFolders(RootPath, FoldersEmpty);

	if (FoldersEmpty.Num() == 0)
	{
		return;
	}

	FScopedSlowTask SlowTaskMain(
		1.0f,
		FText::FromString(TEXT("Deleting empty folders...")),
		bShowSlowTask && GIsEditor && !IsRunningCommandlet()
	);
	SlowTaskMain.MakeDialog(false, false);
	SlowTaskMain.EnterProgressFrame(1.0f);

	FScopedSlowTask SlowTask(
		FoldersEmpty.Num(),
		FText::FromString(TEXT(" ")),
		bShowSlowTask && GIsEditor && !IsRunningCommandlet()
	);
	SlowTask.MakeDialog(false, false);

	bool bErrors = false;

	const int32 NumFoldersTotal = FoldersEmpty.Num();
	int32 NumFoldersDeleted = 0;

	for (const auto& Folder : FoldersEmpty)
	{
		SlowTask.EnterProgressFrame(1.0f, FText::FromString(Folder));

		if (!IFileManager::Get().DeleteDirectory(*Folder, true, true))
		{
			bErrors = true;
			UE_LOG(LogBango, Warning, TEXT("Failed to delete empty folder, unknown reason: %s"), *Folder);
			continue;
		}

		++NumFoldersDeleted;

		// if folder deleted successfully, we must remove it from cached AssetRegistry paths also.
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		AssetRegistryModule.Get().RemovePath(PathConvertToRelative(Folder));
	}

	if (bErrors)
	{
		UE_LOG(LogBango, Error, TEXT("Failed to delete some empty script folders. Please see if OutputLog has more information"));
	}
}

void UBangoEditorSubsystem::GetFolders(const FString& RootPath, TArray<FString>& OutFolders, bool bSearchRecursive)
{
	OutFolders.Empty();

	struct FFindFoldersVisitor : IPlatformFile::FDirectoryVisitor
	{
		TArray<FString>& Folders;

		explicit FFindFoldersVisitor(TArray<FString>& InFolders) : Folders(InFolders) { }

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (bIsDirectory)
			{
				Folders.Emplace(FPaths::ConvertRelativePathToFull(FilenameOrDirectory));
			}

			return true;
		}
	};

	FFindFoldersVisitor FindFoldersVisitor{OutFolders};
	if (bSearchRecursive)
	{
		FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*RootPath, FindFoldersVisitor);
	}
	else
	{
		FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*RootPath, FindFoldersVisitor);
	}
}

void UBangoEditorSubsystem::GetEmptyFolders(const FString& RootPath, TArray<FString>& Folders)
{
	TArray<FString> AllFolders;
	
	GetFolders(RootPath, AllFolders);
	
	Folders.Reserve(AllFolders.Num());
	
	for (const FString& Folder : AllFolders)
	{
		if (IsFolderEmpty(Folder) && !IsFolderEngineGenerated(Folder))
		{
			Folders.Emplace(Folder);
		}
	}
}

bool UBangoEditorSubsystem::IsFolderEmpty(const FString& InPath)
{
	if (InPath.IsEmpty())
	{
		return false;
	}
	
	FName PathRel = FName(PathConvertToRelative(InPath));
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	if (AssetRegistryModule.Get().HasAssets(PathRel, true))
	{
		return false;
	}
	
	FString PathAbs = PathConvertToAbsolute(InPath);
	if (PathAbs.IsEmpty())
	{
		return false;
	}
	
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *PathAbs, TEXT("*"), true, false);
	
	return Files.Num() == 0;
}

bool UBangoEditorSubsystem::IsFolderEngineGenerated(const FString& InPath)
{
	const FString PathDevelopers = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Developers"));
	const FString PathCollections = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) / TEXT("Collections");
	const FString PathCurrentDeveloper = PathDevelopers / FPaths::GameUserDeveloperFolderName();
	const FString PathCurrentDeveloperCollections = PathCurrentDeveloper / TEXT("Collections");

	TSet<FString> EngineGeneratedPaths;
	EngineGeneratedPaths.Emplace(PathDevelopers);
	EngineGeneratedPaths.Emplace(PathCollections);
	EngineGeneratedPaths.Emplace(PathCurrentDeveloper);
	EngineGeneratedPaths.Emplace(PathCurrentDeveloperCollections);

	return EngineGeneratedPaths.Contains(InPath);
}

FString UBangoEditorSubsystem::PathNormalize(const FString& InPath)
{
	if (InPath.IsEmpty())
	{
		return {};
	}
	
	if (!(InPath.StartsWith(TEXT("/")) || InPath.StartsWith(TEXT("\\")) || (InPath.Len() > 2 && InPath[1] == ':')))
	{
		return {};
	}
	
	FString Path = FPaths::ConvertRelativePathToFull(InPath).TrimStartAndEnd();
	FPaths::RemoveDuplicateSlashes(Path);
	FPaths::CollapseRelativeDirectories(Path);
	
	if (FPaths::GetExtension(Path).IsEmpty())
	{
		FPaths::NormalizeDirectoryName(Path);
	}
	else
	{
		FPaths::NormalizeFilename(Path);
	}
	
	if (Path.EndsWith(TEXT("/")) || Path.EndsWith(TEXT("\\")))
	{
		Path = Path.LeftChop(1);
	}
	
	return Path;
}

FString UBangoEditorSubsystem::PathConvertToAbsolute(const FString& InPath)
{
	FString PathNormalized = PathNormalize(InPath);
	FString PathProjectContent = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()).LeftChop(1);
	
	if (PathNormalized.IsEmpty())
	{
		return {};
	}
	
	if (PathNormalized.StartsWith(PathProjectContent))
	{
		return PathNormalized;
	}
	
	if (PathNormalized.StartsWith("/Game"))
	{
		FString Path = PathNormalized;
		Path.RemoveFromStart("/Game");
		
		return Path.IsEmpty() ? PathProjectContent : PathProjectContent / Path;
	}
	
	return {};
}

FString UBangoEditorSubsystem::PathConvertToRelative(const FString& InPath)
{
	FString PathNormalized = PathNormalize(InPath);
	FString PathProjectContent = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir().LeftChop(1));
	
	if (PathNormalized.IsEmpty())
	{
		return {};
	}
	
	if (PathNormalized.StartsWith("/Game"))
	{
		return PathNormalized;
	}
	
	if (PathNormalized.StartsWith(PathProjectContent))
	{
		FString Path = PathNormalized;
		Path.RemoveFromStart("/Game");
		
		return Path.IsEmpty() ? "/Game" : "/Game"/ Path;
	}
	
	return {};
}
