#pragma once

#include "Bango/BangoScriptHolder.h"
#include "IPropertyTypeCustomization.h"

class FBangoScriptHolderCustomization : public IPropertyTypeCustomization
{
public:
	FBangoScriptHolderCustomization();
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	// ------------------------------------------
	
protected:

	TSharedPtr<IPropertyHandle> ScriptBlueprintProperty;
	TSharedPtr<IPropertyHandle> ScriptClassProperty;
	TSharedPtr<IPropertyHandle> GuidProperty;
	TWeakObjectPtr<UEdGraph> CurrentGraph;

	TSharedPtr<SWidget> Box;
	
	TMulticastDelegate<void()> PostScriptCreated;
	TMulticastDelegate<void()> PreScriptDeleted;
	
	// ------------------------------------------
	
	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	int WidgetIndex_CreateDeleteScriptButtons() const;
	
	FReply OnClicked_CreateScript();
	
	FReply OnClicked_DeleteScript();
	
	// ------------------------------------------
	
	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	int WidgetIndex_GraphEditor() const;
	
	FReply OnClicked_EditScript() const;
	
	FReply OnClicked_EnlargeGraphView() const;
	
	TSharedRef<SWidget> GetPopoutGraphEditor() const;
	
	// ------------------------------------------

	void OnPostScriptCreated();

	void OnPreScriptDeleted();	

	void UpdateBox();
	
	// ------------------------------------------

	AActor* GetOwner() const;
	
	UBlueprint* GetBlueprint() const;
	
	UEdGraph* GetPrimaryEventGraph() const;
	
	TSubclassOf<UBangoScriptInstance> GetScriptClass() const;
	
	// ------------------------------------------
	
};