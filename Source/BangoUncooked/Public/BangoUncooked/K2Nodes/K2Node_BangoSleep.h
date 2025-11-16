#pragma once
#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "K2Node_BangoSleep.generated.h"

class UK2Node_TemporaryVariable;

#define LOCTEXT_NAMESPACE "BangoEditor"

UCLASS(MinimalAPI, DisplayName = "Sleep")
class UK2Node_BangoSleep : public UK2NodeBangoBase
{
	GENERATED_BODY()

public:
	UK2Node_BangoSleep();

protected:
	
	UPROPERTY()
	float Duration = 1.0f;
	
	UPROPERTY(EditAnywhere, DisplayName = "Infinite duration")
	bool bInfiniteDuration;
	
	UPROPERTY(EditAnywhere, DisplayName = "Exec Skip")
	bool bEnableSkipExecPin;
	
	UPROPERTY(EditAnywhere, DisplayName = "Exec Cancel")
	bool bEnableCancelExecPin;

	UPROPERTY(EditAnywhere, DisplayName = "Conditional Skip")
	bool bEnableSkipConditionPin;
	
	UPROPERTY(EditAnywhere, DisplayName = "Conditional Cancel")
	bool bEnableCancelConditionPin;

	UPROPERTY(EditAnywhere, DisplayName = "Conditional Pause")
	bool bEnablePausePin;
	
public:
	bool IsInfiniteDuration() const { return bInfiniteDuration; }
	
protected:
	
	UPROPERTY()
	bool bSkipExecTriggered;

	UPROPERTY()
	bool bCancelExecTriggered;
	
	UPROPERTY()
	double FinishTime;

public:
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void AllocateDefaultPins() override;

	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;	

	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	FLinearColor GetNodeTitleColor() const override;
	
	FLinearColor GetNodeTitleTextColor() const override;
	
public:
	bool IsLatentForMacros() const override
	{
		return true;
	}

	FText GetToolTipHeading() const override
	{
		return LOCTEXT("LatentFunc", "Latent");
	}
};

#undef LOCTEXT_NAMESPACE