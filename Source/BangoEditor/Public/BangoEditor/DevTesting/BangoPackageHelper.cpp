#include "BangoEditor/DevTesting/BangoPackageHelper.h"

#include "Misc/ArchiveMD5.h"

/*
UPackage* FBangoPackageHelper::CreateLocalScriptPackage(const UObject* InObjectOuter) // Usually an actor
{
	FGuid UniqueID = FGuid::NewGuid();
	
	// Usually an OFP package in __ExternalActors__
	const UPackage* OutermostPackage = InObjectOuter->IsA<UPackage>() ? CastChecked<UPackage>(InObjectOuter) : InObjectOuter->GetOutermostObject()->GetPackage();

	const FString RootPath = OutermostPackage->GetName();
	const FString ExternalObjectPackageName = GetLocalScriptPackageName(RootPath);
	
	FString FolderShortName = FString("BangoScript") + TEXT("_UID_") + UniqueID.ToString(EGuidFormats::UniqueObjectGuid);
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> GloballyUniqueObjectPath;
	GloballyUniqueObjectPath += OutermostPackage->GetPathName();
	GloballyUniqueObjectPath += TEXT(".");
	GloballyUniqueObjectPath += FolderShortName;
	
	return CreatePackage(*GloballyUniqueObjectPath);
}
*/


FString FBangoPackageHelper::GetLocalScriptPackageName(const FString& InOuterPackageName)
{
	// Convert the object path to lowercase to make sure we get the same hash for case insensitive file systems
	FString ObjectPath = InOuterPackageName.ToLower();

	FArchiveMD5 ArMD5;
	ArMD5 << ObjectPath;

	FGuid PackageGuid = ArMD5.GetGuidFromHash();
	check(PackageGuid.IsValid());

	FString GuidBase36 = PackageGuid.ToString(EGuidFormats::Base36Encoded);
	check(GuidBase36.Len());

	FString BaseDir = FBangoPackageHelper::GetLocalScriptsPath(InOuterPackageName);

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

FString FBangoPackageHelper::GetLocalScriptsPath(const FString& InOuterPackageName, const FString& InPackageShortName)
{
	// Strip the temp prefix if found
	FString ExternalObjectsPath;

	auto TrySplitLongPackageName = [&InPackageShortName](const FString& InOuterPackageName, FString& OutExternalObjectsPath)
	{
		FString MountPoint, PackagePath, ShortName;
		if (FPackageName::SplitLongPackageName(InOuterPackageName, MountPoint, PackagePath, ShortName))
		{
			OutExternalObjectsPath = FString::Printf(TEXT("%s%s/%s%s"), *MountPoint, GetLocalScriptsFolderName(), *PackagePath, InPackageShortName.IsEmpty() ? *ShortName : *InPackageShortName);
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

const TCHAR* FBangoPackageHelper::GetLocalScriptsFolderName()
{
	return TEXT("__BangoLocalScripts__");
}
