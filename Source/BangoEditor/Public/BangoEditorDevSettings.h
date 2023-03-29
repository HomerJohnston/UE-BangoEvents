#pragma once


#include "Engine/DeveloperSettings.h"

#include "BangoEditorDevSettings.generated.h"

// TODO: can I move this to an editor module? Runtime module would depend on editor module though?
UCLASS(Config = Editor, DefaultConfig)
class BANGOEDITOR_API UBangoEditorDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()
};
