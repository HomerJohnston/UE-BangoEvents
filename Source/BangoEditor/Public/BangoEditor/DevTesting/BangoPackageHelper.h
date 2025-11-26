#pragma once

class UBangoScriptObject;

// based loosely on FExternalPackageHelper
class FBangoPackageHelper 
{
public:
	static UPackage* CreateLocalScriptPackage(const UObject* InObjectOuter, UBangoScriptObject*& NewScriptObject);

	static FString GetLocalScriptsPath(const FString& InOuterPackageName, const FString& InPackageShortName = FString());
	static FString GetLocalScriptPackageName(const FString& InOuterPackageName);
	static const TCHAR* GetLocalScriptsFolderName();
};