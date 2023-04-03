#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"

#include "BangoDevSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DisplayName="Bango Event System")
class BANGO_API UBangoDevSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Bango", meta=(ConsoleVariable="Bango.ShowEventsInGame"))
	bool bShowEventsInGame;
};
