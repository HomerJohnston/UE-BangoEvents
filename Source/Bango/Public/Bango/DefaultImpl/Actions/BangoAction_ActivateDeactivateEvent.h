#pragma once

#include "Bango/Action/BangoToggleAction.h"
#include "Bango/Core/BangoInterfaces.h"
#include "Bango/DefaultImpl/BangoDefaultImplEnums.h"


#include "BangoAction_ActivateDeactivateEvent.generated.h"


UCLASS(DisplayName="Activate/Deactivate Event")
class BANGO_API UBangoAction_ActivateDeactivateEvent : public UBangoToggleAction, public IBangoToggleEventActionInterface
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoAction_ActivateDeactivateEvent();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	
	UPROPERTY(EditInstanceOnly, Category="Settings")
	TSoftObjectPtr<ABangoEvent> TargetEvent;
	
	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="OnStart");
	EBangoActivateDeactivateEventAction OnStartAction;

	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="OnStop");
	EBangoActivateDeactivateEventAction OnStopAction;
	
	// ============================================================================================
	// STATE
	// ============================================================================================

	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void OnStart_Implementation() override;
	
	void OnStop_Implementation() override;

private:
	void Execute(EBangoActivateDeactivateEventAction Action);
};
