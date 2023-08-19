#pragma once

#include "IPropertyTypeCustomization.h"

class IDetailGroup;

class FBangoActionPropertyCustomization : public IPropertyTypeCustomization
{
public:
	FBangoActionPropertyCustomization();
	
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	void DrawActionSelector(TSharedRef<IPropertyHandle> PropertyRef, TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils);

	TArray<TSharedPtr<FString>> ComboItems;
	
    TMap<FName, TSharedPtr<STextBlock>> ComboBoxTitleBlocks;
};

