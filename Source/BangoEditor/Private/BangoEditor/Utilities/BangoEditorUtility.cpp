#include "BangoEditor/Utilities/BangoEditorUtility.h"

#include "ObjectTools.h"
#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScript.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Subsystems/BangoEditorSubsystem.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"

FString Bango::Editor::GetGameScriptRootFolder()
{
	return "/Game" / ScriptRootFolder;
}

FString Bango::Editor::GetAbsoluteScriptRootFolder()
{
	return FPaths::ProjectContentDir() / ScriptRootFolder;
}

AActor* Bango::Editor::GetActorOwner(TSharedPtr<IPropertyHandle> Property)
{
	TArray<UObject*> OuterObjects;
	Property->GetOuterObjects(OuterObjects);
	
	if (OuterObjects.Num() > 0)
	{
		if (AActor* Actor = Cast<AActor>(OuterObjects[0]))
		{
			return Actor;
		}
		else if (UActorComponent* Component = Cast<UActorComponent>(OuterObjects[0]))
		{
			return Component->GetOwner();
		}
	}
	
	return nullptr;
}

UPackage* Bango::Editor::MakePackageForScript(UObject* Outer, FString& NewBPName, FGuid Guid)
{
	if (!IsValid(Outer) || Outer->GetFlags() == RF_NoFlags || Outer->HasAnyFlags(RF_BeingRegenerated))
	{
		UE_LOG(LogBango, Error, TEXT("Tried to make a script package but null Outer was passed in!"));
		return nullptr;
	}
	
	UPackage* OuterPackage = Outer->GetPackage();
	
	AActor* Actor = Outer->GetTypedOuter<AActor>();
	
	if (!Actor)
	{
		UE_LOG(LogBango, Error, TEXT("Tried to make a script package for something which does not have an Actor outer! This should never happen."));
		return nullptr;
	}
	
	if (!OuterPackage)
	{
		UE_LOG(LogBango, Error, TEXT("Tried to make a script package for something which does not have a Package! This should never happen."));
		return nullptr;
	}
	
	return MakeScriptPackage_Internal(Actor, OuterPackage, NewBPName, Guid);
}

UPackage* Bango::Editor::MakePackageForScript(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& NewBPName, FGuid Guid)
{
	AActor* Actor = GetActorOwner(ScriptProperty);
	
	if (!Actor)
	{
		return nullptr;
	}
	
	return MakeScriptPackage_Internal(Actor, Outer, NewBPName, Guid);
}

UPackage* Bango::Editor::MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& NewBPName, FGuid Guid)
{
	FString FolderShortName = FString("BangoScript__") + Actor->StaticClass()->GetName() + TEXT("__") + Guid.ToString(EGuidFormats::UniqueObjectGuid);
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> GloballyUniqueObjectPath;
	GloballyUniqueObjectPath += Actor->GetLevel()->GetPathName();
	GloballyUniqueObjectPath += TEXT(".");
	GloballyUniqueObjectPath += FolderShortName;
	
	const UPackage* OutermostPackage = Outer->IsA<UPackage>() ? CastChecked<UPackage>(Outer) : Outer->GetOutermostObject()->GetPackage();
	const FString RootPath = OutermostPackage->GetName();
	FString GuidHashString;
	FString ExternalObjectPackageName = FBangoPackageHelper::GetLocalScriptPackageName(RootPath, FolderShortName, GuidHashString);

	FName Name = "BangoScript"; //Outer->GetFName();
	Name = MakeUniqueObjectName(Outer, UBangoScript::StaticClass(), Name);
	
	FString FinalPath = ExternalObjectPackageName / Name.ToString();// + FPackageName::GetAssetPackageExtension();
	
	//NewBPName = GuidHashString;
	//NewBPName = FString("BangoScript__") + Actor->StaticClass()->GetName() + TEXT("__") + GuidHashString;
	UPackage* NewPackage = CreatePackage(*FinalPath);
	NewPackage->SetFlags(RF_Public);
	NewPackage->SetPackageFlags(PKG_NewlyCreated);
	
	NewBPName = FPackageName::LongPackageNameToFilename(NewPackage->GetName());
	
	return NewPackage;
}

UBangoScriptBlueprint* Bango::Editor::MakeScriptAsset(UPackage* InPackage, FString Name, FGuid Guid)
{
	if (!InPackage)
	{
		return nullptr;
	}
	
	if (Name.IsEmpty())
	{
		return nullptr;
	}
	
	UBangoScriptBlueprint* ScriptBlueprint = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>()->RetrieveDeletedScript(Guid);
	
	if (ScriptBlueprint)
	{
		ScriptBlueprint->Rename(nullptr, InPackage, REN_DontCreateRedirectors);
	}
	else
	{
		ScriptBlueprint = Cast<UBangoScriptBlueprint>(FKismetEditorUtilities::CreateBlueprint(UBangoScript::StaticClass(), InPackage, FName(Name), BPTYPE_Normal, UBangoScriptBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass()));
	}
	
	InPackage->GetOutermost()->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(ScriptBlueprint);

	FKismetEditorUtilities::CompileBlueprint(ScriptBlueprint);
	
	GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateLambda([ScriptBlueprint]()
	{
		ScriptBlueprint->Modify();
		ScriptBlueprint->MarkPackageDirty();
		ScriptBlueprint->GeneratedClass->Modify();
		ScriptBlueprint->GeneratedClass->MarkPackageDirty();
	}));
	
	return ScriptBlueprint;
}

bool Bango::Editor::SaveScriptPackage(UPackage* ScriptPackage, UBlueprint* ScriptBlueprint)
{
	const FString PackageName = ScriptPackage->GetName();
	FString ExistingFilename;
	const bool bPackageAlreadyExists = FPackageName::DoesPackageExist(PackageName, &ExistingFilename);
		
	bool bAttemptSave = true;
        			
	if (!bPackageAlreadyExists)
	{
		const FString& FileExtension = FPackageName::GetAssetPackageExtension();
		ExistingFilename = FPackageName::LongPackageNameToFilename(PackageName, FileExtension);
			
		// Check if we can use this filename.
		FText ErrorText;
		if (!FFileHelper::IsFilenameValidForSaving(ExistingFilename, ErrorText))
		{
			// Display the error (already localized) and exit gracefully.
			FMessageDialog::Open(EAppMsgType::Ok, ErrorText);
			bAttemptSave = false;
		}
	}
		
	if (bAttemptSave)
	{
		FString BaseFilename, Extension, Directory;
		FPaths::NormalizeFilename(ExistingFilename);
		FPaths::Split(ExistingFilename, Directory, BaseFilename, Extension);
			
		FString FinalPackageSavePath = ExistingFilename;
			
		FString FinalPackageFilename = FString::Printf(TEXT("%s.%s"), *BaseFilename, *Extension);
			
		FSavePackageArgs SaveArgs;
		
		return UPackage::SavePackage(ScriptPackage, ScriptBlueprint, /* *PackagePath2 */ *FinalPackageSavePath, SaveArgs);
	}
	
	return false;
}

// TODO erase this, replaced by editor subsystem funcs
bool Bango::Editor::DeleteEmptyFolderFromDisk(const FString& InPathToDelete)
{
	struct FEmptyFolderVisitor : public IPlatformFile::FDirectoryVisitor
	{
		bool bIsEmpty;
		
		TSet<FString> EmptyFolders;

		FEmptyFolderVisitor()
			: bIsEmpty(true)
		{
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (!bIsDirectory)
			{
				bIsEmpty = false;
				return false; // abort searching
			}

			return true; // continue searching
		}
	};

	FString PathToDelete = "/Game/" + InPathToDelete;
	
	FString PathToDeleteOnDisk;
	if (FPackageName::TryConvertLongPackageNameToFilename(PathToDelete, PathToDeleteOnDisk))
	{
		// Look for files on disk in case the folder contains things not tracked by the asset registry
		FEmptyFolderVisitor EmptyFolderVisitor;
		IFileManager::Get().IterateDirectoryRecursively(*PathToDeleteOnDisk, EmptyFolderVisitor);

		if (EmptyFolderVisitor.bIsEmpty)
		{
			return IFileManager::Get().DeleteDirectory(*PathToDeleteOnDisk, false, true);
		}
	}

	return false;
}

void Bango::Editor::NewScriptRequested(UObject* Outer, FBangoScriptContainer* ScriptContainer, FGuid Guid)
{
}
