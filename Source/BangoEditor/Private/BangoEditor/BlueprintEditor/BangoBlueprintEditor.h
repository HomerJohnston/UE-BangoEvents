#pragma once

#include "BlueprintEditor.h"

class FBangoBlueprintEditor : public FBlueprintEditor
{
public:
	
	/** Setup all the events that the graph editor can handle */
	void SetupGraphEditorEvents_Impl(UBlueprint* Blueprint, UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents);

	void Tick(float DeltaTime) override;
	//FActionMenuContent OnCreateGraphActionMenu_Impl(UEdGraph* InGraph, const FVector2f& InNodePosition, const TArray<UEdGraphPin*>& InDraggedPins, bool bAutoExpand, SGraphEditor::FActionMenuClosed InOnMenuClosed);
	
	float InstructionsFadeCountdown;
};
