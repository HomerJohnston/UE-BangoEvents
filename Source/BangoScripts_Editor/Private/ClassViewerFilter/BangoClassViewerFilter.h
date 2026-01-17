#pragma once

#include "ClassViewerFilter.h"

/**
 * This class filters out level script from dialogs like New Blueprint or class selection dropdowns.
 */
class FBangoClassViewerFilter : public IClassViewerFilter
{
public:
	FBangoClassViewerFilter(TSharedPtr<IClassViewerFilter> InChildFilter);
	
protected:
	
	TSharedPtr<IClassViewerFilter> ChildFilter;

public:

	bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs);
	
	bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const class IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs) override;
};

