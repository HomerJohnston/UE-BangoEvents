#pragma once

#include "Puny/Action.h"

#include "Action_InstigateEvent.generated.h"

class UPunyEvent;
class APlayerController;
struct FPunyEventSignal;
class UCanvas;
enum class EPunyTriggerSignalType : uint8;

UCLASS(DisplayName="Activate/Deactivate Event")
class BANGO_API UPunyAction_InstigateEvent : public UPunyAction
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================

public:
	UPunyAction_InstigateEvent();
	
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
	EPunyTriggerSignalType OnStart;
	
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	EPunyTriggerSignalType OnStop;
	
public:
	void HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal) override;

private:
	void Handle(EPunyTriggerSignalType Signal);

	void HandleComponent(EPunyTriggerSignalType Signal);

	void HandleActor(EPunyTriggerSignalType Signal);

public:
	virtual bool HasValidSetup() override;
	
	virtual void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;
};