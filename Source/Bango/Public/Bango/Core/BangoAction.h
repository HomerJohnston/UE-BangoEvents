#pragma once

#include "BangoAction.generated.h"

class ABangoEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRun, UObject*, InInstigator);

UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// Settings
	// ============================================================================================
private:
	UPROPERTY(EditAnywhere)
	bool bIgnoreEventDelay = false;

	UPROPERTY(EditAnywhere)
	double Delay = 0;
	
	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------
	
	// ============================================================================================
	// State
	// ============================================================================================
private:
	UPROPERTY(Transient)
	FTimerHandle DelayHandle;

public:
	UPROPERTY(Transient)
	FOnRun OnRunEvent;
	
	UPROPERTY(Transient)
	FOnFailed OnRunFailed;
	
	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------
public:
	void RunInternal(ABangoEvent* EventActor, UObject* Instigator, double EventDelay);
	
	void PerformRun(ABangoEvent* EventActor, UObject* Instigator);
	
	/** Performs action logic. */
	UFUNCTION(BlueprintNativeEvent)
	void Run(ABangoEvent* EventActor, UObject* Instigator);

	/** This can be called if the instigator becomes invalid during a delayed run. */
	UFUNCTION(BlueprintNativeEvent)
	void OnFailToRun(ABangoEvent* EventActor);
};
