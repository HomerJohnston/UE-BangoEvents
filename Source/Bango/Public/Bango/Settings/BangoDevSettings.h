// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "BangoDevSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DisplayName="Bango Event System")
class BANGO_API UBangoDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	// TODO: console variables for quicker hiding?
public:
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	bool bShowEventsInGame = false;
	
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	bool bShowEventsInEditor = true;

	/** Beyond this distance, all event data will be hidden */
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	float DisplayDistanceFar = 3000;

	/** Beyond this distance, detailed event data will be hidden */
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	float DisplayDistanceClose = 1000;
};
