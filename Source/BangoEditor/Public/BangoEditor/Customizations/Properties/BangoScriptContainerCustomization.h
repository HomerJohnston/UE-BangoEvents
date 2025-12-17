#pragma once

#include "Bango/Core/BangoScriptContainer.h"
#include "IPropertyTypeCustomization.h"

class FBangoScriptContainerCustomization : public IPropertyTypeCustomization
{
public:
	FBangoScriptContainerCustomization();
	
	~FBangoScriptContainerCustomization();
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	// ------------------------------------------
	
protected:

	//TSharedPtr<IPropertyHandle> ScriptBlueprintProperty;
	TSharedPtr<IPropertyHandle> ScriptClassProperty;
	TSharedPtr<IPropertyHandle> GuidProperty;
	TWeakObjectPtr<UEdGraph> CurrentGraph;

	TSharedPtr<SVerticalBox> Box;
	
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
	
	FReply OnClicked_RenameScript() const;
	
	TSharedRef<SWidget> GetPopoutGraphEditor() const;
	
	// ------------------------------------------

	void OnPostScriptCreatedOrRenamed();

	void OnPreScriptDeleted();	

	void UpdateBox();
	
	// ------------------------------------------

	AActor* GetOwner() const;
	
	UObject* GetOuter() const;
	
	UBlueprint* GetBlueprint() const;
	
	UEdGraph* GetPrimaryEventGraph() const;
	
	TSubclassOf<UBangoScript> GetScriptClass() const;
	
	// ------------------------------------------
	
};