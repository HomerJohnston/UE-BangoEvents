// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Puny/Action.h"

#include "Action_FreezeThawEvent.generated.h"

UENUM()
enum class EPunyFreezeThawEventAction : uint8
{
	FreezeEvent,
	UnfreezeEvent,
	DoNothing,
};

UCLASS()
class BANGO_API UPunyAction_FreezeThawEvent : public UPunyAction
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================

public:
	UPunyAction_FreezeThawEvent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

private:
	// DisplayName = "Use Overlap Events From Component", 
	/** Optionally choose a specific component to listen for overlap triggers from. Note:  */
	UPROPERTY(Category="Settings", EditAnywhere)
	FComponentReference TargetComponent;
	
private:
	UPROPERTY(Category="Settings", EditAnywhere)
	EPunyFreezeThawEventAction OnStart;

	UPROPERTY(Category="Settings", EditAnywhere)
	EPunyFreezeThawEventAction OnStop;
	
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
	virtual void HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal) override;

private:
	void Handle(EPunyFreezeThawEventAction Action);
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
#endif
};