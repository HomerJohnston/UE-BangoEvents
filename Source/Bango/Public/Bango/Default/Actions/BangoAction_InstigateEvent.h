#pragma once

#include "Bango/BangoAction.h"

#include "BangoAction_InstigateEvent.generated.h"

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

public:
	void Start_Implementation(UBangoEvent* Event, UObject* Instigator) override;

	void Stop_Implementation(UBangoEvent* Event, UObject* Instigator) override;
	
private:
	void HandleComponent(EBangoTriggerSignalType Signal);

	void HandleActor(EBangoTriggerSignalType Signal);

#if WITH_EDITOR
public:
	virtual bool HasValidSetup() override;
	
	virtual void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;
#endif
};