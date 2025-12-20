#include "SBangoGraphEditor.h"

void SBangoGraphEditor::Construct(const FArguments& InArgs)
{
	check(InArgs._BlueprintEditor);
	BlueprintEditor = InArgs._BlueprintEditor;
	
	ChildSlot
	[
		SNew(SGraphEditor)
		.IsEditable(InArgs._IsEditable)
		.GraphToEdit(InArgs._GraphToEdit)
		.GraphEvents(InArgs._GraphEvents)
		.ShowGraphStateOverlay(InArgs._ShowGraphStateOverlay)
	];
}
