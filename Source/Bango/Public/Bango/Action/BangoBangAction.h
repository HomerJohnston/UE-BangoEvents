// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Action/BangoAction.h"
#include "BangoBangAction.generated.h"

/** Optional "Bang" action, usable only in Bang events. Has start delay feature built-in. */
UCLASS(Abstract, Within="BangoEvent_Bang")
class BANGO_API UBangoBangAction : public UBangoAction
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

protected:
	void StartDelayed();
};
