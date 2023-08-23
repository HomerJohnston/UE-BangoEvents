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

	void GenerateComboboxEntries(TSharedRef<IPropertyHandle> PropertyHandle);

	TArray<FName> ActionFunctionNames;
	TArray<TSharedPtr<FString>> ActionFunctionDescriptions;
	TArray<FLinearColor> ActionFunctionNameColors;
	
    TMap<FName, TSharedPtr<STextBlock>> ComboBoxTitleBlocks;
};

