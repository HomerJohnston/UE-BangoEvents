#pragma once

#include "Bango/Core/BangoTrigger.h"

#include "BangoTrigger_EventActivated.generated.h"

enum class EBangoActivateDeactivateEventAction : uint8;

UCLASS(DisplayName="Event Activated")
class BANGO_API UBangoTrigger_EventActivated : public UBangoTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_EventActivated();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:

	UPROPERTY(EditInstanceOnly, Category="Settings")
	TSoftObjectPtr<ABangoEvent> TargetEvent;

	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="OnStart");
	EBangoActivateDeactivateEventAction OnEventActivatedAction;

	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="OnStop");
	EBangoActivateDeactivateEventAction OnEventDeactivatedAction;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

private:
	UFUNCTION()
	void OnTargetEventActivated(ABangoEvent* Event, UObject* Instigator);

	UFUNCTION()
	void OnTargetEventDeactivated(ABangoEvent* Event, UObject* Instigator);
	
	void Execute(EBangoActivateDeactivateEventAction Action);
};