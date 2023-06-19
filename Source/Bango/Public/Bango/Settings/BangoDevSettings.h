// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "BangoDevSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DisplayName="Bango Event System")
class BANGO_API UBangoDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	// TODO: console variables for quicker hiding?
protected:
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	bool bShowEventsInGame = false;
	
	/** Beyond this distance, all event data will be hidden */
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	float FarDisplayDistance = 5000;

	/** Beyond this distance, detailed event data will be hidden */
	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	float NearDisplayDistance = 2500;

	UPROPERTY(Config, EditAnywhere, Category = "Bango", meta=(UIMin=0.5, UIMax=5.0))
	float EventDisplaySize = 1.0;

	UPROPERTY(Config, EditAnywhere, Category = "Bango")
	TSoftObjectPtr<UMaterial> CustomMeshMaterial;
	
public:
	UFUNCTION(BlueprintCallable)
	bool GetShowEventsInGame() const;

	UFUNCTION(BlueprintCallable)
	float GetFarDisplayDistance() const;

	UFUNCTION(BlueprintCallable)
	float GetNearDisplayDistance() const;

	UFUNCTION(BlueprintCallable)
	float GetEventDisplaySize() const;

	UFUNCTION(BlueprintCallable)
	UMaterial* GetCustomMeshMaterial() const;
};
