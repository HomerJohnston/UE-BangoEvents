// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Action.h"

#include "Action_FreezeThawEvent.generated.h"

UENUM()
enum class EBangoFreezeThawEventAction : uint8
{
	FreezeEvent,
	UnfreezeEvent,
	DoNothing,
};

UCLASS(DisplayName="Freeze/Thaw Event")
class BANGO_API UBangoAction_FreezeThawEvent : public UBangoAction
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================

public:
	UBangoAction_FreezeThawEvent();
	
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
	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoFreezeThawEventAction OnStart;

	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoFreezeThawEventAction OnStop;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------

	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------

	// ============================================================================================
	// METHODS
	// ============================================================================================

public:
	virtual void HandleSignal_Implementation(UBangoEvent* Event, FBangoEventSignal Signal) override;

private:
	void Handle(EBangoFreezeThawEventAction Action);

	void HandleComponent(bool Val);

	void HandleActor(bool Val);
	
	// ============================================================================================
	// EDITOR_SETTINGS
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
#if WITH_EDITOR
public:
	void DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont) override;

	virtual void AppendDebugData(TArray<FBangoDebugTextEntry>& Data);

	bool HasValidSetup() override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
