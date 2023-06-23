// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "BangoDevSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DisplayName="Bango Event System")
class BANGO_API UBangoDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	void PostCDOContruct() override;
	
	
	// TODO: console variables for quicker hiding?
protected:
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	bool bShowEventsInGame = false;
	
	/** Beyond this distance, all event data will be hidden */
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	float FarDisplayDistance = 10000;

	/** Beyond this distance, detailed event data will be hidden */
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	float NearDisplayDistance = 5000;

	UPROPERTY(Config, EditAnywhere, Category = "Bango", meta=(UIMin=0.5, UIMax=5.0))
	float EventDisplaySize = 2.0;

public:
	UFUNCTION(BlueprintCallable)
	bool GetShowEventsInGame() const;

	UFUNCTION(BlueprintCallable)
	float GetFarDisplayDistance() const;

	UFUNCTION(BlueprintCallable)
	float GetNearDisplayDistance() const;

	UFUNCTION(BlueprintCallable)
	float GetEventDisplaySize() const;

	void OnCvarChange();

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
