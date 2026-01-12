#include "BangoEditor/Commands/BangoEditorActions.h"

#include "BangoEditor/BangoEditorStyle.h"
#include "Framework/Commands/Commands.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

FBangoEditorCommands::FBangoEditorCommands()
	: ::TCommands<FBangoEditorCommands>
	(
		"Bango",
		LOCTEXT("BangoEditor", "Bango Editor"),
		NAME_None,
		FBangoEditorStyle::GetStyleSetName()
	)
{
	
}

void FBangoEditorCommands::RegisterCommands()
{
	UI_COMMAND(SetEditActorID, "SetEditBangoActorID", "Set or Edit Bango Actor ID", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::I));
}

#undef LOCTEXT_NAMESPACE