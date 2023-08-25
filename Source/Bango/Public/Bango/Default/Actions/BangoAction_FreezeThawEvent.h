// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/BangoAction.h"

#include "BangoAction_FreezeThawEvent.generated.h"

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
	UPROPERTY(EditAnywhere, meta=(EditCondition="bUseTargetActor"))
	AActor* TargetActor;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseTargetComponent = false;
	
	/** Freeze/thaw the specified event. Note: only works on self actor due to a bug in FComponentReference. */
	UPROPERTY(EditAnywhere, meta=(EditCondition = "bUseTargetComponent", UseComponentPicker))
	FComponentReference TargetComponent;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------
#if WITH_EDITORONLY_DATA
protected:
#endif
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
	void Freeze(UBangoEvent* Event, UObject* Instigator);

	void Unfreeze(UBangoEvent* Event, UObject* Instigator);
	
private:
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
