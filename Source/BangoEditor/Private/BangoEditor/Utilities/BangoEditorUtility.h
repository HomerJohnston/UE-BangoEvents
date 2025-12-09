#pragma once
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dialogs/Dialogs.h"
#include "Kismet2/KismetEditorUtilities.h"

class UBangoScriptBlueprint;
class UBangoScriptComponent;
class UBangoScript;

namespace Bango::Editor
{
	static FString ScriptRootFolder = TEXT("__BangoScripts__");
	
	FString GetGameScriptRootFolder();
	
	FString GetAbsoluteScriptRootFolder();
	
	AActor* GetActorOwner(TSharedPtr<IPropertyHandle> Property);
	
	UPackage* MakeScriptPackage(UBangoScriptComponent* Component, UObject* Outer, FString& NewBPName);

	UPackage* MakeScriptPackage(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& NewBPName);
	
	UPackage* MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& NewBPName);
	
	UBangoScriptBlueprint* MakeScriptAsset(UPackage* InPackage, FString Name, FGuid Guid);
	
	bool SaveScriptPackage(UPackage* ScriptPackage, UBlueprint* ScriptBlueprint);
	
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScript> ScriptClass);
	
	// I can't find any existing PUBLIC code in this retarded engine to do this, so now I have to copy code from AssetViewUtils
	bool DeleteEmptyFolderFromDisk(const FString& InPathToDelete);
	
	
	
	static void CreateBlueprint(const FString& InBlueprintName, UClass* InCreationClass)
	{
		if(InCreationClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(InCreationClass))
		{
			FMessageDialog::Open( EAppMsgType::Ok, NSLOCTEXT("UnrealEd", "InvalidClassToMakeBlueprintFrom", "Invalid class to make a Blueprint of."));
			return;
		}

		// Get the full name of where we want to create the physics asset.
		FString PackageName = InBlueprintName;

		// Then find/create it.
		UPackage* Package = CreatePackage( *PackageName);
		check(Package);

		// Handle fully loading packages before creating new objects.
		TArray<UPackage*> TopLevelPackages;
		TopLevelPackages.Add( Package->GetOutermost() );
		if( !UPackageTools::HandleFullyLoadingPackages( TopLevelPackages, NSLOCTEXT("UnrealEd", "CreateANewObject", "Create a new object") ) )
		{
			// Can't load package
			return;
		}

		FName BPName(*FPackageName::GetLongPackageAssetName(PackageName));

		if(PromptUserIfExistingObject(BPName.ToString(), PackageName, Package))
		{
			// Create and init a new Blueprint
			UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(InCreationClass, Package, BPName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), FName("ClassViewer"));
			if(NewBP)
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(NewBP);

				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(NewBP);

				// Mark the package dirty...
				Package->MarkPackageDirty();
			}
		}

		// All viewers must refresh.
		//ClassViewer::Helpers::PopulateClassviewerDelegate.Broadcast();
	}

}
