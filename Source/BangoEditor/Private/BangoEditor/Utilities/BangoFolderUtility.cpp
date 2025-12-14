#include "BangoFolderUtility.h"

#include "AssetToolsModule.h"
#include "BangoEditorUtility.h"
#include "IAssetTools.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Bango/Core/BangoScript.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Utility/BangoLog.h"

void Bango::Editor::DeleteEmptyScriptFolders()
{
	DeleteEmptyFolders(Bango::Editor::GetAbsoluteScriptRootFolder());
}

void Bango::Editor::DeleteEmptyFolders(const FString& RootPath, bool bShowSlowTask)
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

void Bango::Editor::GetFolders(const FString& RootPath, TArray<FString>& OutFolders, bool bSearchRecursive)
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

void Bango::Editor::GetEmptyFolders(const FString& RootPath, TArray<FString>& Folders)
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

bool Bango::Editor::IsFolderEmpty(const FString& InPath)
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

bool Bango::Editor::IsFolderEngineGenerated(const FString& InPath)
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

FString Bango::Editor::PathNormalize(const FString& InPath)
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

FString Bango::Editor::PathConvertToAbsolute(const FString& InPath)
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

FString Bango::Editor::PathConvertToRelative(const FString& InPath)
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

void Bango::Editor::DeleteUnreferencedScripts()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetData> BangoScriptAssets;
	AssetRegistryModule.Get().GetAssetsByClass(UBangoScriptBlueprint::StaticClass()->GetClassPathName(), BangoScriptAssets);
	
	TArray<FAssetData> UnreferencedScripts;
	
	for (FAssetData& AssetData : BangoScriptAssets)
	{
		if (AssetData.GetObjectPathString().StartsWith(Bango::Editor::GetGameScriptRootFolder()))
		{
			TArray<FName> Referencers;
			AssetRegistryModule.Get().GetReferencers(AssetData.GetPackage()->GetFName(), Referencers);
			
			if (Referencers.Num() == 0)
			{
				UnreferencedScripts.Add(AssetData);
			}
		}
	}

	ObjectTools::DeleteAssets(UnreferencedScripts, false);
}
