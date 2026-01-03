#pragma once

#include "BlueprintEditor.h"

class FBangoBlueprintEditor : public FBlueprintEditor
{
public:
	void SetupGraphEditorEvents(UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents) override;
	
	/** Setup all the events that the graph editor can handle */
	void SetupGraphEditorEvents_Impl(UBlueprint* Blueprint, UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents);

	void Tick(float DeltaTime) override;
	//FActionMenuContent OnCreateGraphActionMenu_Impl(UEdGraph* InGraph, const FVector2f& InNodePosition, const TArray<UEdGraphPin*>& InDraggedPins, bool bAutoExpand, SGraphEditor::FActionMenuClosed InOnMenuClosed);
	
	void OnDropActor(const TArray<TWeakObjectPtr<AActor>>& Actors, UEdGraph* EdGraph, const UE::Math::TVector2<double>& Vector2) const;

	void OnDropActors(const TArray<TWeakObjectPtr<AActor>>& Actors, UEdGraph* EdGraph, const UE::Math::TVector2<float>& Vector2) const;
	
	// Sadly Epic didn't expose enough functionality for me to prevent adding multiple graphs 
	//bool AreEventGraphsAllowed() const override { return false; }
	FGraphAppearanceInfo GetGraphAppearance(class UEdGraph* InGraph) const override;
	
	FText GetOwnerNameAsText() const;

	float OpenTime;
	
	FText WarningText;
	
	void SetWarningText(const FText& InText);
	
protected:
	void PostInitAssetEditor() override;
};
