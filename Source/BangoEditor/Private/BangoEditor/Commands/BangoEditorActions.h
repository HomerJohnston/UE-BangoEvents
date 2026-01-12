#pragma once

#include "Framework/Commands/Commands.h"
#include "Templates/SharedPointer.h"

class FBangoEditorCommands : public TCommands<FBangoEditorCommands>
{
public:
	FBangoEditorCommands();
	
	TSharedPtr<FUICommandInfo> SetEditActorID;
	
	void RegisterCommands() override;
};
