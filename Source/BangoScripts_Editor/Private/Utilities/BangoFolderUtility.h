#pragma once

#include "Containers/UnrealString.h"
#include "UObject/SoftObjectPtr.h"

class UBangoScript;

namespace Bango::Editor
{
	void DeleteEmptyLevelScriptFolders();
	void DeleteEmptyFolders(const FString& RootPath, bool bShowSlowTask = false);
	
	void GetFolders(const FString& RootPath, TArray<FString>& OutFolders, bool bSearchRecursive = true);
	void GetEmptyFolders(const FString& RootPath, TArray<FString>& Folders);

	bool IsFolderEmpty(const FString& InPath);
	bool IsFolderEngineGenerated(const FString& InPath);

	FString PathNormalize(const FString& InPath);
	FString PathConvertToAbsolute(const FString& InPath);
	FString PathConvertToRelative(const FString& InPath);
	
	void DeleteUnreferencedScripts();
	
	bool IsScriptReferenced(TSoftClassPtr<UBangoScript> ScriptClass);
	
	void DeleteScriptIfUnreferenced(TSoftClassPtr<UBangoScript> ScriptClass);
}
