// Copyright Epic Games, Inc. All Rights Reserved.

#include "BangoPackageHelper.h"

#if WITH_EDITOR

#include "AssetRegistry/IAssetRegistry.h"
#include "Misc/ArchiveMD5.h"
#include "Misc/Paths.h"
#include "HAL/PlatformApplicationMisc.h"
#include "UObject/MetaData.h"
#include "GameFramework/Actor.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"

FBangoPackageHelper::FOnObjectPackagingModeChanged FBangoPackageHelper::OnObjectPackagingModeChanged;

FString FBangoPackageHelper::GetLocalScriptsPath(const FString& InOuterPackageName, const FString& InPackageShortName)
{
	// Strip the temp prefix if found
	FString ExternalObjectsPath;

	auto TrySplitLongPackageName = [&InPackageShortName](const FString& InOuterPackageName, FString& OutExternalObjectsPath)
	{
		FString MountPoint, PackagePath, ShortName;
		if (FPackageName::SplitLongPackageName(InOuterPackageName, MountPoint, PackagePath, ShortName))
		{
			FString ExternalActorsPrefix(FPackagePath::GetExternalActorsFolderName());
			
			if (PackagePath.StartsWith(ExternalActorsPrefix))
			{
				PackagePath.RightChopInline(ExternalActorsPrefix.Len());
			}
			
			OutExternalObjectsPath = FString::Printf(TEXT("%s%s%s%s"), *MountPoint, *Bango::Editor::ScriptRootFolder, *PackagePath, InPackageShortName.IsEmpty() ? *ShortName : *InPackageShortName);
			
			return true;
		}
		return false;
	};

	// This exists only to support the Fortnite Foundation Outer streaming which prefix a valid package with /Temp (/Temp/Game/...)
	// Unsaved worlds also have a /Temp prefix but no other mount point in their paths and they should fallback to not stripping the prefix. (first call to SplitLongPackageName will fail and second will succeed)
	if (InOuterPackageName.StartsWith(TEXT("/Temp")))
	{
		FString BaseOuterPackageName = InOuterPackageName.Mid(5);
		if (TrySplitLongPackageName(BaseOuterPackageName, ExternalObjectsPath))
		{
			return ExternalObjectsPath;
		}
	}

	if (TrySplitLongPackageName(InOuterPackageName, ExternalObjectsPath))
	{
		return ExternalObjectsPath;
	}

	return FString();
}

FString FBangoPackageHelper::GetLocalScriptsPath(UPackage* InPackage, const FString& InPackageShortName /* = FString()*/, bool bTryUsingPackageLoadedPath /* = false*/)
{
	check(InPackage);
	if (bTryUsingPackageLoadedPath && !InPackage->GetLoadedPath().IsEmpty())
	{
		return FBangoPackageHelper::GetLocalScriptsPath(InPackage->GetLoadedPath().GetPackageName());
	}
	// We can't use the Package->FileName here because it might be a duplicated a package
	// We can't use the package short name directly in some cases either (PIE, instanced load) as it may contain pie prefix or not reflect the real object location
	return FBangoPackageHelper::GetLocalScriptsPath(InPackage->GetName(), InPackageShortName);
}

FString FBangoPackageHelper::GetLocalScriptPackageName(const FString& InOuterPackageName, const FString& InObjectPath, FString& GuidBase36)
{
	// Convert the object path to lowercase to make sure we get the same hash for case insensitive file systems
	FString ObjectPath = InObjectPath.ToLower();

	FArchiveMD5 ArMD5;
	ArMD5 << ObjectPath;

	FGuid PackageGuid = ArMD5.GetGuidFromHash();
	check(PackageGuid.IsValid());

	GuidBase36 = PackageGuid.ToString(EGuidFormats::Base36Encoded);
	check(GuidBase36.Len());

	FString BaseDir = FBangoPackageHelper::GetLocalScriptsPath(InOuterPackageName);
	
	/*
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> ObjectPackageName;
	ObjectPackageName.Append(BaseDir);
	ObjectPackageName.Append(TEXT("/"));
	ObjectPackageName.Append(*GuidBase36);
	return ObjectPackageName.ToString();
	*/
	
	// TODO for some reason this is necessary to hide assets from the Content view. I'd really rather use the format above.
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> ObjectPackageName;
	ObjectPackageName.Append(BaseDir);
	ObjectPackageName.Append(TEXT("/"));
	ObjectPackageName.Append(*GuidBase36, 1);
	ObjectPackageName.Append(TEXT("/"));
	ObjectPackageName.Append(*GuidBase36 + 1, 2);
	ObjectPackageName.Append(TEXT("/"));
	ObjectPackageName.Append(*GuidBase36 + 3);
	return ObjectPackageName.ToString();
}

FString FBangoPackageHelper::GetLocalScriptPackageInstanceName(const FString& OuterPackageName, const FString& ObjectPackageName)
{
	return FLinkerInstancingContext::GetInstancedPackageName(OuterPackageName, ObjectPackageName);
}

void FBangoPackageHelper::GetSortedAssets(const FARFilter& Filter, TArray<FAssetData>& OutAssets)
{
	OutAssets.Reset();
	IAssetRegistry::GetChecked().GetAssets(Filter, OutAssets);
	OutAssets.Sort();
}


#endif
