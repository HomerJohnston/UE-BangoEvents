#pragma once

#include "UObject/Object.h"

#include "Bango/Core/BangoSignal.h"

#include "BangoTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTriggerDelegate, EBangoSignal, Signal, UObject*, NewInstigator);

UENUM(BlueprintType)
enum class EBangoTriggerInstigatorAction : uint8
{
	AddInstigator,
	RemoveInstigator,
	DoNothing,
};

class ABangoEvent;
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoTrigger : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	
public:
	// TODO can I private this and expose binding functions?
	/** Call this delegate to control the event. */
	UPROPERTY()
	FTriggerDelegate TriggerSignal;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Get Event")
	ABangoEvent* GetEventBP();

	ABangoEvent* GetEvent();
	
	template<class T>
	T* GetEvent();
	
public:
	UFUNCTION(BlueprintCallable)
	void SetEnabled(bool bEnabled);

protected:
	/** Perform setup to make the trigger work properly here (e.g. subscribe to events in your game, or turn on a timer to check things, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Enable();
	
	/** Run code to make the trigger stop working here (unsubscribe to events, stop timers, etc). */
	UFUNCTION(BlueprintNativeEvent)
	void Disable();

protected:
	UFUNCTION(BlueprintCallable)
	void SendTriggerSignal(EBangoSignal Signal, UObject* NewInstigator);

#if WITH_EDITOR
public:
	virtual FText GetDisplayName();
#endif
};

template <class T>
T* UBangoTrigger::GetEvent()
{
	return Cast<T>(GetOuter());
}
