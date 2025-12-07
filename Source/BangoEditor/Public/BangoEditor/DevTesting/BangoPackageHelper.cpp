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

/*
EPackageFlags FBangoPackageHelper::GetDefaultBangoPackageFlags()
{
	return (PKG_EditorOnly | PKG_ContainsMapData | PKG_NewlyCreated);
}

FBangoPackageHelper::FRenameExternalObjectsHelperContext::FRenameExternalObjectsHelperContext(const UObject* SourceObject, ERenameFlags Flags)
{
	if (GIsEditor && ((Flags & REN_Test) == 0))
	{
		check(SourceObject);
		const UPackage* Package = SourceObject->GetPackage();
		check(Package);
		SourcePackage = Package;
		OldObject = SourceObject;
	}
}

FBangoPackageHelper::FRenameExternalObjectsHelperContext::~FRenameExternalObjectsHelperContext()
{	
	if (GIsEditor && OldObject && (OldObject->GetPackage() != SourcePackage))
	{		
		// Get external packages
		const TArray<UPackage*>& ExternalObjectPackages = OldObject->GetPackage()->GetBangoPackages();
		for (const UPackage* BangoPackage : ExternalObjectPackages)
		{
			TArray<UObject*> DependantObjects;
			ForEachObjectWithPackage(BangoPackage, [&DependantObjects](UObject* Object)
			{
				check(Object)
				DependantObjects.Add(Object);
				return true;
			}, false);

			for (UObject* Object : DependantObjects)
            {
			    FBangoPackageHelper::SetPackagingMode(Object, nullptr, false);
			    FBangoPackageHelper::SetPackagingMode(Object, OldObject, true);
            }
		}
	}
}
*/

#if 0
void FBangoPackageHelper::DuplicateLocalScriptPackages(const UObject* InObject, FObjectDuplicationParameters& InDuplicationParameters, EActorPackagingScheme ActorPackagingScheme /*= EActorPackagingScheme::Reduced*/)
{
	if (InDuplicationParameters.DuplicateMode != EDuplicateMode::PIE && InDuplicationParameters.bAssignExternalPackages)
	{
		const UPackage* SourcePackage = InObject->GetPackage();
		const FString SourcePackageName = SourcePackage->GetName();
		const FString SourcePackageExternalActorsPath = ULevel::GetExternalActorsPath(SourcePackageName);
		const FString SourcePackageExternalObjectsPath = GetLocalScriptsPath(SourcePackageName);
		UPackage* DestinationPackage = InDuplicationParameters.DestOuter->GetPackage();
		
		FString ReplaceFrom = FPaths::GetBaseFilename(*SourcePackage->GetName());
		ReplaceFrom = FString::Printf(TEXT("%s.%s:"), *ReplaceFrom, *ReplaceFrom);

		FString ReplaceTo = FPaths::GetBaseFilename(*DestinationPackage->GetName());
		ReplaceTo = FString::Printf(TEXT("%s.%s:"), *ReplaceTo, *ReplaceTo);
			
		ForEachObjectWithOuter(InObject, [&ReplaceFrom, &ReplaceTo, &InDuplicationParameters, SourcePackage, DestinationPackage, ActorPackagingScheme](const UObject* Object)
		{
			if (UPackage* Package = Object ? Object->GetBangoPackage() : nullptr)
			{
				const FString PackageName = Package->GetName();

				FString SplitPackageRoot;
				FString SplitPackagePath;
				FString SplitPackageName;
				if (FPackageName::SplitLongPackageName(PackageName, SplitPackageRoot, SplitPackagePath, SplitPackageName))
				{
					if (SplitPackagePath.StartsWith(FPackagePath::GetExternalActorsFolderName()) || SplitPackagePath.StartsWith(GetBangoScriptsFolderName()))
					{
						FString Path = Object->GetPathName();
						if (DestinationPackage != SourcePackage)
						{
							Path = Path.Replace(*ReplaceFrom, *ReplaceTo);
						}
						UPackage* DupPackage = Object->IsA<AActor>() ? ULevel::CreateActorPackage(DestinationPackage, ActorPackagingScheme, Path, Object) : FBangoPackageHelper::CreateLocalScriptPackage(DestinationPackage, Path);
						DupPackage->MarkAsFullyLoaded();
						DupPackage->MarkPackageDirty();
				
						InDuplicationParameters.DuplicationSeed.Add(Package, DupPackage);
					}
				}
			}
		}, /*bIncludeNestedObjects*/ true);
	}
}
#endif
	
UPackage* FBangoPackageHelper::CreateLocalScriptPackage(const UObject* InObjectOuter, const FString& InObjectPath, EPackageFlags InFlags, const UExternalDataLayerAsset* InExternalDataLayerAsset)
{
	const UPackage* OutermostPackage = InObjectOuter->IsA<UPackage>() ? CastChecked<UPackage>(InObjectOuter) : InObjectOuter->GetOutermostObject()->GetPackage();
	const FString RootPath = InExternalDataLayerAsset ? FExternalDataLayerHelper::GetExternalDataLayerLevelRootPath(InExternalDataLayerAsset, OutermostPackage->GetName()) : OutermostPackage->GetName();
	const FString ExternalObjectPackageName = FBangoPackageHelper::GetLocalScriptPackageName(RootPath, InObjectPath);
	UPackage* Package = CreatePackage(*ExternalObjectPackageName);
	Package->SetPackageFlags(InFlags);
	return Package;
}

/*
void FBangoPackageHelper::SetPackagingMode(UObject* InObject, const UObject* InObjectOuter, bool bInIsPackageExternal, bool bInShouldDirty, EPackageFlags InBangoPackageFlags)
{
	if (bInIsPackageExternal == InObject->IsPackageExternal())
	{
		return;
	}

	// Optionally mark the current object & package as dirty
	InObject->Modify(bInShouldDirty);

	if (bInIsPackageExternal)
	{
		const IDataLayerInstanceProvider* DataLayerInstanceProvider = InObject->GetImplementingOuter<IDataLayerInstanceProvider>();
		const UExternalDataLayerAsset* ExternalDataLayerAsset = DataLayerInstanceProvider ? DataLayerInstanceProvider->GetRootExternalDataLayerAsset() : nullptr;
		UPackage* NewObjectPackage = FBangoPackageHelper::CreateBangoPackage(InObjectOuter, InObject->GetPathName(), InBangoPackageFlags, ExternalDataLayerAsset);
		InObject->SetBangoPackage(NewObjectPackage);
	}
	else
	{
		UPackage* ObjectPackage = InObject->GetBangoPackage();
		// Detach the linker exports so it doesn't resolve to this object anymore
		ResetLinkerExports(ObjectPackage);
		InObject->SetBangoPackage(nullptr);
	}

	OnObjectPackagingModeChanged.Broadcast(InObject, bInIsPackageExternal);

	// Mark the new object package dirty
	InObject->MarkPackageDirty();
}
*/

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

FString FBangoPackageHelper::GetLocalScriptPackageName(const FString& InOuterPackageName, const FString& InObjectPath)
{
	// Convert the object path to lowercase to make sure we get the same hash for case insensitive file systems
	FString ObjectPath = InObjectPath.ToLower();

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

FString FBangoPackageHelper::GetLocalScriptPackageInstanceName(const FString& OuterPackageName, const FString& ObjectPackageName)
{
	return FLinkerInstancingContext::GetInstancedPackageName(OuterPackageName, ObjectPackageName);
}

#if 0
void FBangoPackageHelper::GetExternalSaveableObjects(UObject* InOuter, TArray<UObject*>& OutObjects, EGetExternalSaveableObjectsFlags InFlags)
{
	// Get external packages
	TSet<UPackage*> ExternalObjectPackages;
	ExternalObjectPackages.Append(InOuter->GetPackage()->GetBangoPackages());

	// Find assets for external packages
	for (UPackage* BangoPackage : ExternalObjectPackages)
	{
		const bool bPassesDirtyCheck = !EnumHasAnyFlags(InFlags, EGetExternalSaveableObjectsFlags::CheckDirty) || BangoPackage->IsDirty();
		if(bPassesDirtyCheck && FPackageName::IsValidLongPackageName(BangoPackage->GetName()))
		{
			if(UObject* Asset = BangoPackage->FindAssetInPackage())
			{
				OutObjects.Add(Asset);
			}
		}
	}
}
#endif 

#if 0
TArray<FString> FBangoPackageHelper::GetObjectsBangoPackageFilePath(const TArray<const UObject*>& InObjects)
{
	TArray<FString> PackageFilePaths;
	for (const UObject* Object : InObjects)
	{
		if (Object && Object->IsPackageExternal())
		{
			const FString LocalFullPath(Object->GetBangoPackage()->GetLoadedPath().GetLocalFullPath());
			if (!LocalFullPath.IsEmpty())
			{
				PackageFilePaths.Add(FPaths::ConvertRelativePathToFull(LocalFullPath));
			}
		}
	}
	return PackageFilePaths;
}
#endif

#if 0
void FBangoPackageHelper::CopyObjectsBangoPackageFilePathToClipboard(const TArray<const UObject*>& InObjects)
{
	TArray<FString> PackageFilePaths = GetObjectsBangoPackageFilePath(InObjects);
	if (!PackageFilePaths.IsEmpty())
	{
		for (FString& PackageFilePath : PackageFilePaths)
		{
			FPaths::MakePlatformFilename(PackageFilePath);
		}
		FString Result = FString::Join(PackageFilePaths, LINE_TERMINATOR);
		check(Result.Len());
		FPlatformApplicationMisc::ClipboardCopy(*Result);
	}
}
#endif

void FBangoPackageHelper::GetSortedAssets(const FARFilter& Filter, TArray<FAssetData>& OutAssets)
{
	OutAssets.Reset();
	IAssetRegistry::GetChecked().GetAssets(Filter, OutAssets);
	OutAssets.Sort();
}


#endif
