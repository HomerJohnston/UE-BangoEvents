#pragma once

#include "UObject/Object.h"

#include "TriggerCondition.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTrigger, UObject*, NewInstigator);

class ABangoEvent;
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoTriggerCondition : public UObject
{
	GENERATED_BODY()

public:
	/** The owning ABangoEvent will listen for this delegate to fire. */
	UPROPERTY()
	FOnTrigger OnTrigger;
	
public:
	UFUNCTION(BlueprintCallable)
	ABangoEvent* GetEvent();
	
	void SetEnabled(bool bEnabled);
	
	/** Perform setup to make the trigger valid and active here (subscribe to events or turn on a tick timer etc). */ // TODO enforce implementation of this!
	UFUNCTION(BlueprintNativeEvent)
	void Enable();
	
	/** Run code to make the trigger stop working here (unsubscribe to events or stop ticks etc). */ // TODO enforce implementation of this!
	UFUNCTION(BlueprintNativeEvent)
	void Disable();

	/** Run this function to fire (start or stop) the ABangoEvent that owns this trigger. */
	UFUNCTION(BlueprintCallable)
	void Trigger(UObject* NewInstigator);
};