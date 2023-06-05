#pragma once

#include "UObject/Object.h"

#include "BangoTrigger.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FTriggerDelegate, UObject*, NewInstigator);

class ABangoEvent;
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoTrigger : public UObject
{
	GENERATED_BODY()

private:
	/** The owning ABangoEvent will listen for this delegate to fire. */
	UPROPERTY()
	FTriggerDelegate OnTriggerActivation;

	/** The owning ABangoEvent will listen for this delegate to fire. */
	UPROPERTY()
	FTriggerDelegate OnTriggerDeactivation;
	
protected:
	UFUNCTION(BlueprintCallable)  // TODO pure?
	ABangoEvent* GetEvent();

public:
	void BindEvent(ABangoEvent* Event);

	UFUNCTION(BlueprintCallable)
	void SetEnabled(bool bEnabled);

protected:
	/** Perform setup to make the trigger valid and active here (subscribe to events or turn on a tick timer etc). */ // TODO enforce implementation of this!
	UFUNCTION(BlueprintNativeEvent)
	void Enable();
	
	/** Run code to make the trigger stop working here (unsubscribe to events or stop ticks etc). */ // TODO enforce implementation of this!
	UFUNCTION(BlueprintNativeEvent)
	void Disable();

protected:
	/** Run this function to fire (start or stop) the ABangoEvent that owns this trigger. */
	UFUNCTION(BlueprintCallable)
	void ActivateEvent(UObject* NewInstigator);

	UFUNCTION(BlueprintCallable)
	void DeactivateEvent(UObject* OldInstigator);
};