#pragma once

namespace Bango::Editor
{
	static void DeleteEmptyLevelScriptFolders();
	static void DeleteEmptyFolders(const FString& RootPath, bool bShowSlowTask = false);
	
	static void GetFolders(const FString& RootPath, TArray<FString>& OutFolders, bool bSearchRecursive = true);
	static void GetEmptyFolders(const FString& RootPath, TArray<FString>& Folders);

	static bool IsFolderEmpty(const FString& InPath);
	static bool IsFolderEngineGenerated(const FString& InPath);

	static FString PathNormalize(const FString& InPath);
	static FString PathConvertToAbsolute(const FString& InPath);
	static FString PathConvertToRelative(const FString& InPath);
	
	
	static void DeleteUnreferencedScripts();
}
