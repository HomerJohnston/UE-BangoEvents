// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoEvent.h"

#include "BangoEvent_Toggle.generated.h"

class UBangoTrigger_Toggle;
class UBangoAction_Toggle;

UENUM(BlueprintType)
enum class EBangoToggleDeactivateCondition : uint8
{
	AllInstigatorsRemoved,
	AnyInstigatorsRemoved,
	OriginalInstigatorRemoved,
	AnyDeactivateRequest,
	MAX						UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EBangoToggleState : uint8
{
	Null,
	Activated,
	Deactivated,
};

UCLASS()
class BANGO_API ABangoEvent_Toggle : public ABangoEvent
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	ABangoEvent_Toggle();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================

	// TODO details customization to remove this bullshit
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="1==2", EditConditionHides))
	uint8 IReallyWantTheDisplayCategoryToAppearFirstAndIHateMyLife;
#endif
	
protected:
	/** Determines how the event can be deactivated. */
	UPROPERTY(Category="Bango|Event", EditAnywhere, DisplayName="Deactivate When")
	EBangoToggleDeactivateCondition DeactivateCondition;
	
	UFUNCTION(BlueprintCallable)
	EBangoToggleDeactivateCondition GetDeactivateCondition() const;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
private:
	UPROPERTY(Transient)
	EBangoToggleState ToggleState = EBangoToggleState::Deactivated;
	
	// ------------------------------------------
	// State Getters and Setters
	// ------------------------------------------

	EBangoToggleState GetToggleState();

	bool SetToggleState(EBangoToggleState NewState, UObject* ByInstigator);
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	virtual bool ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator) override;

protected:
	virtual bool HasInvalidData() const override;

protected:
	bool Activate(UObject* ActivateInstigator);

	bool Deactivate(UObject* DeactivateInstigator);

	void SignalActions(EBangoSignal Signal, UObject* StartInstigator);

	virtual void ResetRemainingTriggerLimits() override;
	
public:
	void SetFrozen(bool bFreeze) override;

	void PerformPendingFreeze(ABangoEvent* Event, EBangoSignal Signal, UObject* NewInstigator);

public:
#if WITH_EDITOR
	void UpdateProxyState() override;

	FLinearColor GetColorBase() const override;
	
	FLinearColor GetColorForProxy() const override;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Game() const override;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Editor() const override;
#endif
};