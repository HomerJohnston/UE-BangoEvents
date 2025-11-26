#pragma once

#include "Bango/BangoScriptHolder.h"
#include "IPropertyTypeCustomization.h"

class FBangoScriptHolderCustomization : public IPropertyTypeCustomization
{
public:
	FBangoScriptHolderCustomization();
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
protected:

	TSharedPtr<IPropertyHandle> ScriptClassProperty;
	
	
	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	FReply OnClicked_CreateScript();
	
	FReply OnClicked_EditScript();
	
	TSubclassOf<UBangoScriptObject> GetScriptClass() const;
	
	UPackage* MakeLocalScriptPackage(UObject* Outer, FString& NewBPName);
};