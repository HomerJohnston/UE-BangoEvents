#include "BangoEditorUtility.h"

#include "ExternalPackageHelper.h"
#include "ObjectTools.h"
#include "PropertyHandle.h"
#include "BangoScripts/Core/BangoScriptBlueprint.h"
#include "BangoScripts/Core/BangoScript.h"
#include "BangoScripts/Utility/BangoScriptsLog.h"
#include "BangoScripts/EditorTooling/BangoScriptsEditorLog.h"
#include "HAL/FileManager.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"
#include "WorldPartition/WorldPartition.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

// ----------------------------------------------

FString Bango::Editor::GetGameScriptRootFolder()
{
	return TEXT("__BangoScripts__");
}

// ----------------------------------------------

FString Bango::Editor::GetAbsoluteScriptRootFolder()
{
	return FPaths::ProjectContentDir() / GetGameScriptRootFolder();
}

// ----------------------------------------------

UPackage* Bango::Editor::MakeLevelScriptPackage(UObject* Outer, FGuid Guid)
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

// ----------------------------------------------

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

// ----------------------------------------------

UPackage* Bango::Editor::MakeLevelScriptPackage_Internal(AActor* Actor, UObject* Outer, FGuid Guid)
{
	check(Actor && Outer && Guid.IsValid());
	
	// The goal here is a file path like this
	// /Game/__BangoScripts__/LevelName/ActorPath/UniqueScriptID.uasset

	FString LevelName = FPackageName::GetShortName(Actor->GetLevel()->GetPackage());
	
	// I am not concerned with collisions; most actors in a game will have one script. Might as well reduce the path length to make it nicer.
	uint32 GuidHash = GetTypeHash(Guid);
	FString GuidHashBase36 = UInt32ToBase36(GuidHash);
	
	FString ActorFolderPath;
	
	UWorld* World = Actor->GetWorld();
	check(World);
	
	if (World->GetWorldPartition())
	{
		// We'll build a subfolder hierarchy that contains the same folders as __ExternalActors__ for world partition. This may make it easier to match up script files to actors e.g. using the revision control window.
		FString ObjectPath = Actor->GetPathName().ToLower();
		FString BaseDir = FExternalPackageHelper::GetExternalObjectsPath(Actor->GetPackage()->GetPathName());
	
		FString ExternalPackageName = FExternalPackageHelper::GetExternalPackageName(Actor->GetPackage()->GetPathName(), ObjectPath);
		ExternalPackageName.RemoveFromStart(BaseDir);
		
		FString ActorClass = Actor->GetClass()->GetFName().ToString();
		ActorFolderPath = ActorClass / ExternalPackageName;
	}
	else
	{
		FString ActorFName = Actor->GetFName().ToString();
		
		ActorFolderPath = ActorFName;
		//ActorFolderPath.RemoveFromStart(ActorClass);
	}
	
	check(!ActorFolderPath.IsEmpty());

	// Generate the preferred final path name
	FString FinalPath = "/Game" / Bango::Editor::GetGameScriptRootFolder() / LevelName / ActorFolderPath / GuidHashBase36;
	
	// Ensure we have a unique pathname
	uint32 Suffix = 0;
	FString OriginalFinalPath = FinalPath;
	while (FPackageName::DoesPackageExist(FinalPath))
	{
		UE_LOG(LogBangoEditor, Warning, TEXT("A level script package already existed, this should not happen! Existing path: %s"), *FinalPath);
		FinalPath = FString::Format(TEXT("{0}_{1}"), { OriginalFinalPath, ++Suffix} );
	}
	
	// Make the package
	UPackage* NewPackage = CreatePackage(*FinalPath);
	NewPackage->SetFlags(RF_Public);
	NewPackage->SetPackageFlags(PKG_NewlyCreated);
	
	FString PackageName = FPackageName::LongPackageNameToFilename(NewPackage->GetName());
	
	return NewPackage;
}

// ----------------------------------------------

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
	
	FAssetRegistryModule::AssetCreated(ScriptBlueprint);

	FKismetEditorUtilities::CompileBlueprint(ScriptBlueprint);
	
	return ScriptBlueprint;
}

// ----------------------------------------------

UBangoScriptBlueprint* Bango::Editor::DuplicateLevelScript(UBangoScriptBlueprint* SourceBlueprint, UPackage* NewScriptPackage, const FString& InName, const FGuid& NewGuid, AActor* NewOwnerActor)
{
	FString ScriptName = InName;
	
	if (ScriptName.IsEmpty())
	{
		ScriptName = FPackageName::GetShortName(NewScriptPackage);
	}
	
	FString AssetName = GetLocalScriptName(ScriptName);
	
	UBangoScriptBlueprint* DuplicateScript = DuplicateObject(SourceBlueprint, NewScriptPackage, FName(AssetName));
	
	DuplicateScript->Reset();
	
	DuplicateScript->SetScriptGuid(NewGuid);
	DuplicateScript->SetActorReference(NewOwnerActor);	
	
	return DuplicateScript;
}

// ----------------------------------------------

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
