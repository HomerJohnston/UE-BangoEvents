#pragma once

#include "IPropertyTypeCustomization.h"

class IDetailGroup;

struct FGroupProperties
{
	FGroupProperties();
	FGroupProperties(FName InGroupName);
	
	FName GroupName;
	TArray<TSharedRef<IPropertyHandle>> Properties;
};

class FBangoPropertyDrawer
{
public:
	FBangoPropertyDrawer() {};
	virtual ~FBangoPropertyDrawer() {};
	
	virtual void Draw(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder) {};
};

class FBangoActionTriggerPropertyCustomizationBase : public IPropertyTypeCustomization
{
public:
	FBangoActionTriggerPropertyCustomizationBase();

protected:
	FLinearColor HeaderColor;
	
	TMap<FName, TSharedPtr<FBangoPropertyDrawer>> PropertyDrawers;
	
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void DrawGroup(FGroupProperties& Group, IDetailChildrenBuilder& ChildBuilder, bool bUseGroup);
};
