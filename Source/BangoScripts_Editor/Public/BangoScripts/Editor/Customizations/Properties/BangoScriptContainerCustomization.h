#pragma once

#include "BangoScripts/Core/BangoScriptContainer.h"
#include "IPropertyTypeCustomization.h"
#include "Input/Reply.h"

class SVerticalBox;
class FBangoBlueprintEditor;

enum class EBangoScriptRenameStatus
{
	ValidNewName,
	MatchesCurrent,
	MatchesOther,
	InvalidNewName,
};

class FBangoScriptContainerCustomization : public IPropertyTypeCustomization
{
public:
	FBangoScriptContainerCustomization();
	
	~FBangoScriptContainerCustomization();
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	// ------------------------------------------
	
protected:
	TSharedPtr<IPropertyHandle> ScriptContainerProperty;
	TSharedPtr<IPropertyHandle> ScriptClassProperty;
	TSharedPtr<IPropertyHandle> GuidProperty;
	TWeakObjectPtr<UEdGraph> CurrentGraph;

	TSharedPtr<SVerticalBox> Box;
	
	TMulticastDelegate<void()> PostScriptCreated;
	TMulticastDelegate<void()> PreScriptDeleted;
		
	FText ScriptNameText;
	
	EBangoScriptRenameStatus ProposedNameStatus;
	
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
	
	void OnTextChanged_ScriptNameEditableText(const FText& Text);
	
	void SetProposedScriptName(const FText& Text);
	
	EBangoScriptRenameStatus GetProposedNameStatus();
	
	FSlateColor ForegroundColor_ScriptNameEditableText() const;
	
	FSlateColor FocusedForegroundColor_ScriptNameEditableText() const;
	
	bool IsEnabled_RenameScriptButton() const;
	
	TSharedRef<SWidget> GetPopoutGraphEditor(FVector2D WindowSize) const;
	
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
	
	void OnScriptContainerDestroyed(UObject* Object, FBangoScriptContainer* ScriptContainer);
	
	void OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap);
};