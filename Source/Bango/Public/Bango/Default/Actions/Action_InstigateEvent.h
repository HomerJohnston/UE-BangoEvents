#pragma once

#include "Bango/Action.h"

#include "Action_InstigateEvent.generated.h"

class UBangoEvent;
class APlayerController;
struct FBangoEventSignal;
class UCanvas;
enum class EBangoTriggerSignalType : uint8;

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
	EBangoTriggerSignalType OnStart;
	
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoTriggerSignalType OnStop;
	
public:
	void HandleSignal_Implementation(UBangoEvent* Event, FBangoEventSignal Signal) override;

private:
	void Handle(EBangoTriggerSignalType Signal);

	void HandleComponent(EBangoTriggerSignalType Signal);

	void HandleActor(EBangoTriggerSignalType Signal);

public:
	virtual bool HasValidSetup() override;
	
	virtual void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;
};