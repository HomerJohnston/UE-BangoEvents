#pragma once

#include "ActionTriggerPropertyCustomizationBase.h"
#include "IPropertyTypeCustomization.h"

class IDetailGroup;

class FBangoActionPropertyCustomization_ActionSelectorDrawer : public FBangoPropertyDrawer
{
	void Draw(TSharedRef<IPropertyHandle> PropertyRef, IDetailChildrenBuilder& ChildBuilder) override;

	void GenerateComboboxEntries(TSharedPtr<IPropertyHandle> PropertyHandle);
	
	TArray<FName> ActionFunctionNames;

	TArray<TSharedPtr<FString>> ActionFunctionDescriptions;
	
	TMap<FName, TSharedPtr<STextBlock>> ComboBoxTitleBlocks;
};

class FBangoActionPropertyCustomization : public FBangoActionTriggerPropertyCustomizationBase
{
public:
	FBangoActionPropertyCustomization();
	
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};
