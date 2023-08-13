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
	EBangoEventSignalType OnStart;
	
	/**  */
	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoEventSignalType OnStop;
	
public:
	void HandleSignal_Implementation(UBangoEvent* Event, FBangoActionSignal Signal) override;

private:
	void Handle(EBangoEventSignalType Signal);

	void HandleComponent(EBangoEventSignalType Signal);

	void HandleActor(EBangoEventSignalType Signal);

public:
	virtual bool HasValidSetup() override;
	
	virtual void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;
};