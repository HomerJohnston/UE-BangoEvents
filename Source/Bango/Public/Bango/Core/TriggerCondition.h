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
	UPROPERTY()
	FOnTrigger OnTrigger;
	
public:
	/** Run code to make the trigger work here (subscribe to events or turn on a tick timer etc). */ // TODO enforce implementation of this!
	UFUNCTION(BlueprintNativeEvent)
	void Setup(ABangoEvent* Event);

public:
	/** Run code to make the trigger stop working here (unsubscribe to events or stop ticks etc). */ // TODO enforce implementation of this!
	UFUNCTION(BlueprintNativeEvent)
	void Freeze(ABangoEvent* Event);
};