#include "BangoEditor/Utilities/BangoEditorUtility.h"

#include "ObjectTools.h"
#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Core/BangoScriptObject.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Subsystems/BangoEditorSubsystem.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"

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

UPackage* Bango::Editor::MakeScriptPackage(UBangoScriptComponent* Component, UObject* Outer, FString& NewBPName)
{
	AActor* Actor = Component->GetOwner();
	
	if (!Actor)
	{
		return nullptr;
	}
	
	return MakeScriptPackage_Internal(Actor, Outer, NewBPName);
}

UPackage* Bango::Editor::MakeScriptPackage(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& NewBPName)
{
	AActor* Actor = GetActorOwner(ScriptProperty);
	
	if (!Actor)
	{
		return nullptr;
	}
	
	return MakeScriptPackage_Internal(Actor, Outer, NewBPName);
}

UPackage* Bango::Editor::MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& NewBPName)
{
	FGuid UniqueID = FGuid::NewGuid();
	
	FString FolderShortName = FString("BangoScript") + TEXT("_UID_") + UniqueID.ToString(EGuidFormats::UniqueObjectGuid);
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> GloballyUniqueObjectPath;
	GloballyUniqueObjectPath += Actor->GetLevel()->GetPathName();
	GloballyUniqueObjectPath += TEXT(".");
	GloballyUniqueObjectPath += FolderShortName;
	
	const UPackage* OutermostPackage = Outer->IsA<UPackage>() ? CastChecked<UPackage>(Outer) : Outer->GetOutermostObject()->GetPackage();
	const FString RootPath = OutermostPackage->GetName();
	const FString ExternalObjectPackageName = FBangoPackageHelper::GetLocalScriptPackageName(RootPath, FolderShortName);

	FString FinalPath = ExternalObjectPackageName;// + FPackageName::GetAssetPackageExtension();
	
	NewBPName = FolderShortName;
	return CreatePackage(*FinalPath);
}

UBlueprint* Bango::Editor::MakeScriptAsset(UPackage* InPackage, FString Name, FGuid Guid)
{
	if (!InPackage)
	{
		return nullptr;
	}
	
	if (Name.IsEmpty())
	{
		return nullptr;
	}
	
	UBlueprint* ScriptBlueprint = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>()->RetrieveSoftDeletedScript(Guid);
	
	if (ScriptBlueprint)
	{
		ScriptBlueprint->Rename(nullptr, InPackage, REN_DontCreateRedirectors);
	}
	else
	{
		ScriptBlueprint = FKismetEditorUtilities::CreateBlueprint(UBangoScriptInstance::StaticClass(), InPackage, FName(Name), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
		ScriptBlueprint->SetFlags(RF_Public);	
	}
	
	ScriptBlueprint->Modify();
	InPackage->SetDirtyFlag(true);
	
	FKismetEditorUtilities::CompileBlueprint(ScriptBlueprint);
	
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
