#pragma once

#include "Bango/Action.h"

#include "Action_InstigateEvent.generated.h"

class UBangoEvent;
class APlayerController;
struct FBangoActionSignal;
class UCanvas;
enum class EBangoEventSignalType : uint8;

UCLASS(DisplayName="Activate/Deactivate Event")
class BANGO_API UBangoAction_InstigateEvent : public UBangoAction
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================

public:
	UBangoAction_InstigateEvent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

private:
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetActor = true;

	/** Freeze/thaw all events on a specified actor. TODO: need a way to limit to a single component. */
	UPROPERTY(Category="Settings", EditAnywhere, meta=(EditCondition="bUseTargetActor"))
	AActor* TargetActor;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetComponent = false;
	
	/** Freeze/thaw the specified event. Note: only works on self actor due to a bug in FComponentReference. */
	UPROPERTY(Category="Settings", EditAnywhere, meta=(EditCondition = "bUseTargetComponent", UseComponentPicker))
	FComponentReference TargetComponent;

private:
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoEventSignalType OnStartAction;
	
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoEventSignalType OnStopAction;
	
public:
	void OnStart_Implementation(UBangoEvent* Event, UObject* Instigator) override;

	void OnStop_Implementation(UBangoEvent* Event, UObject* Instigator) override;
	
private:
	void Handle(EBangoEventSignalType Signal);

	void HandleComponent(EBangoEventSignalType Signal);

	void HandleActor(EBangoEventSignalType Signal);

public:
	virtual bool HasValidSetup() override;
	
	virtual void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;
};