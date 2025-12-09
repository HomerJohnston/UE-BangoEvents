#include "BangoEditor/ClassViewerFilter/BangoClassViewerFilter.h"

#include "Bango/Core/BangoScript.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"

FBangoClassViewerFilter::FBangoClassViewerFilter(TSharedPtr<IClassViewerFilter> InChildFilter)
{
	ChildFilter = InChildFilter;
}

bool FBangoClassViewerFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs)
{
	if (ChildFilter && !ChildFilter->IsClassAllowed(InInitOptions, InClass, InFilterFuncs))
	{
		return false;
	}
	
	/*
	if (InClass == UBangoScript::StaticClass())
	{
		return true;
	}
	return !InClass->IsChildOf(UBangoScript::StaticClass());	
	*/
	
	if (InClass->IsChildOf(UBangoScript::StaticClass()))
	{
		if (UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(InClass))
		{
			UPackage* BlueprintPackage = Blueprint->GetPackage();
			
			if (BlueprintPackage == GetTransientPackage())
			{
				return false;
			}
			
			FString PackagePath = FPackageName::GetLongPackagePath(BlueprintPackage->GetName());
			
			return !PackagePath.StartsWith(Bango::Editor::GetGameScriptRootFolder());
		}
	}
	
	// TODO study existing implementations and flesh this out properly
	
	return true;
}

bool FBangoClassViewerFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const class IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs)
{
	if (ChildFilter && !ChildFilter->IsUnloadedClassAllowed(InInitOptions, InUnloadedClassData, InFilterFuncs))
	{
		return false;
	}
	
	// TODO can I make this more efficient?
	if (InUnloadedClassData->GetClassName()->Equals(UBangoScript::StaticClass()->GetFName().ToString()))
	{
		return true;
	}
	
	return !InUnloadedClassData->IsChildOf(UBangoScript::StaticClass());
	
	// TODO study existing implementations and flesh this out properly
}
