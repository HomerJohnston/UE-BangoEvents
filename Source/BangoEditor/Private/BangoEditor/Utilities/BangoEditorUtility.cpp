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
#include "WorldPartition/WorldPartition.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

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

UPackage* Bango::Editor::MakeLevelScriptPackage(UObject* Outer, /*FString& InOutBPName, */FGuid Guid)
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
	
	return MakeLevelScriptPackage_Internal(Actor, OuterPackage, /*InOutBPName, */Guid);
}

// I may later need this for manually creating scripts on FBangoScriptContainers manually
/*
UPackage* Bango::Editor::MakeLevelScriptPackage(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& InOutBPName, FGuid Guid)
{
	AActor* Actor = GetActorOwner(ScriptProperty);
	
	if (!Actor)
	{
		return nullptr;
	}
	
	return MakeScriptPackage_Internal(Actor, Outer, InOutBPName, Guid);
}
*/

FString UInt32ToBase36(uint32 Value)
{
	static const TCHAR Alphabet[] = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	FString Result;
	do
	{
		Result.InsertAt(0, Alphabet[Value % 36]);
		Value /= 36;
	}
	while (Value > 0);

	return Result;
}

UPackage* Bango::Editor::MakeLevelScriptPackage_Internal(AActor* Actor, UObject* Outer, FGuid Guid)
{
	check(Actor && Outer && Guid.IsValid());
	
	// The goal here is a file path like this
	// /Game/__BangoScripts__/LevelName/ActorPath/UniqueScriptID/Script.uasset

	FString FinalPath;

	// Not sure which of these I need yet
	//FString BaseDir = FBangoPackageHelper::GetWorldPartitionLocalScriptsPath();
	FString LevelName = FPackageName::GetShortName(Actor->GetLevel()->GetPackage());
	FString ActorPath = Actor->GetPathName();
	FString ActorName = Actor->GetName();
	FString ScriptID = Guid.ToString();
	FString ScriptID1 = Guid.ToString(EGuidFormats::Base36Encoded);
	FString ScriptID2 = Guid.ToString(EGuidFormats::Digits);
	FString ScriptID3 = Guid.ToString(EGuidFormats::DigitsLower);
	FString ScriptID4 = Guid.ToString(EGuidFormats::HexValuesInBraces);
	FString ScriptID5 = Guid.ToString(EGuidFormats::Short);
	FString ScriptID6 = Guid.ToString(EGuidFormats::UniqueObjectGuid);
	
	// TODO I do want to actually test a collision and make sure the plugin handles it somewhat gracefully
	// I am not concerned with collisions; most actors in a game will have one script. Might as well reduce the path length to make it nicer.
	uint32 GuidHash = GetTypeHash(Guid);
	FString GuidHashBase36 = UInt32ToBase36(GuidHash);
	
	FArchiveMD5 MD5;
	MD5 << Guid;
	
	FName ActorFName = Actor->GetFName();
	
	FString ActorFNameString = ActorFName.ToString();
	
	FString ActorClass = Actor->GetClass()->GetFName().ToString();
	
	ActorFNameString.RemoveFromStart(ActorClass);
	ActorFNameString.RemoveFromStart(TEXT("_"));
	
	FinalPath = "/Game" / Bango::Editor::ScriptRootFolder / LevelName / ActorClass / ActorFNameString / GuidHashBase36;
	
	UPackage* NewPackage = CreatePackage(*FinalPath);
	NewPackage->SetFlags(RF_Public);
	NewPackage->SetPackageFlags(PKG_NewlyCreated);
	
	FString PackageName = FPackageName::LongPackageNameToFilename(NewPackage->GetName());
	
	return NewPackage;
}

UBangoScriptBlueprint* Bango::Editor::GetDeletedLevelScript(const FGuid& Guid)
{
	UBangoScriptBlueprint* ScriptBlueprint = GEditor->GetEditorSubsystem<UBangoLevelScriptsEditorSubsystem>()->RetrieveDeletedLevelScript(Guid);

	//ScriptBlueprint->Rename(*ScriptBlueprint->RetrieveDeletedName(), InPackage, REN_DontCreateRedirectors);
	
	return ScriptBlueprint;
}

UBangoScriptBlueprint* Bango::Editor::MakeLevelScript(UPackage* InPackage, const FString& InName, const FGuid& InScriptGuid)
{
	if (!InPackage)
	{
		return nullptr;
	}
	
	if (InName.IsEmpty())
	{
		return nullptr;
	}
	
	FString AssetName = GetLocalScriptName(InName);
	UBangoScriptBlueprint* ScriptBlueprint = Cast<UBangoScriptBlueprint>(FKismetEditorUtilities::CreateBlueprint(UBangoScript::StaticClass(), InPackage, FName(AssetName), BPTYPE_Normal, UBangoScriptBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass()));
	ScriptBlueprint->SetScriptGuid(InScriptGuid);
	
//	InPackage->GetOutermost()->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(ScriptBlueprint);

	FKismetEditorUtilities::CompileBlueprint(ScriptBlueprint);
	
	/*
	GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateLambda([ScriptBlueprint]()
	{
		ScriptBlueprint->Modify();
		ScriptBlueprint->MarkPackageDirty();
		ScriptBlueprint->GeneratedClass->Modify();
		ScriptBlueprint->GeneratedClass->MarkPackageDirty();
	}));
	*/
	
	return ScriptBlueprint;
}

UBangoScriptBlueprint* Bango::Editor::DuplicateLevelScript(UBangoScriptBlueprint* SourceBlueprint, UPackage* NewScriptPackage, const FString& InName, const FGuid& NewGuid)
{
	FString ScriptName = InName;
	
	if (ScriptName.IsEmpty())
	{
		ScriptName = FPackageName::GetShortName(NewScriptPackage);
	}
	
	FString AssetName = GetLocalScriptName(ScriptName);
	
	UBangoScriptBlueprint* DuplicateScript = DuplicateObject(SourceBlueprint, NewScriptPackage, FName(AssetName));
	DuplicateScript->SetScriptGuid(NewGuid);
	
	return DuplicateScript;
}

#if 0
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
#endif

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

FString Bango::Editor::GetLocalScriptName(FString InName)
{
	// We append a funny character to the UObject name to make it invisible in the content browser (this is a hacky hack). Note that a period '.' is not allowed because of some filepath checks in engine code.
	return TEXT("~") + InName;
	
	// \U0001F4DC Manuscript / Scroll
	// \U0001F9FE Receipt
	// \U0001F4A5 Explosion
	// \U0001F4CD Pushpin
	// \U0000FE6B Small Form @
	// \U0000FF5E Halfwidth Forms ~
}
