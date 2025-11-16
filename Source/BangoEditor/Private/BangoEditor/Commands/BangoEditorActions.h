#pragma once

class FBangoEditorCommands : public TCommands<FBangoEditorCommands>
{
public:
	FBangoEditorCommands();
	
	TSharedPtr<FUICommandInfo> SetEditActorID;
	
	void RegisterCommands() override;
	
	
};