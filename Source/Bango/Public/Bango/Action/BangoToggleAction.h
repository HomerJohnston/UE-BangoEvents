// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Action/BangoAction.h"
#include "BangoToggleAction.generated.h"

/** Optional "Toggle" action, usable only in Toggle events. Has start/stop delay features built-in. */
UCLASS(Abstract, Within="BangoEvent_Toggle")
class BANGO_API UBangoToggleAction : public UBangoAction
{
	GENERATED_BODY()
	
	// ============================================================================================
	// Settings
	// ============================================================================================
protected:
	/** When set, starting of action will be delayed by the specified length of time. */
	UPROPERTY(Category="Action Settings", EditAnywhere, meta=(EditCondition="bUseStartDelay", ClampMin = 0.0))
	double StartDelay = 0;

	UPROPERTY()
	bool bUseStartDelay = false;

	/** If set, prevents this action's OnStart function from running. */
	UPROPERTY(Category="Action Settings", EditAnywhere)
	bool bBlockFromStarting = false;
	
	/** When set, stopping of action will be delayed by the specified length of time. */
	UPROPERTY(Category="Action Settings", EditAnywhere, meta=(EditCondition="bUseStopDelay", ClampMin = 0.0))
	double StopDelay = 0;

	UPROPERTY()
	bool bUseStopDelay = false;
	
	/** If set, prevents this action's OnStop function from running. */
	UPROPERTY(Category="Action Settings", EditAnywhere)
	bool bBlockFromStopping = false;
	
	// ------------------------------------------
	// Settings Getters and Setters
	// ------------------------------------------
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	
	// ------------------------------------------
	// State Getters and Setters
	// ------------------------------------------

	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Start(UObject* StartInstigator); // override;

	void Stop(UObject* StopInstigator); // override;

protected:
	void StartDelayed();

	void StopDelayed();
};