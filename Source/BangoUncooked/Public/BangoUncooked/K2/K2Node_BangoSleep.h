#pragma once
#include "BangoUncooked/K2/_K2NodeBangoBase.h"

#include "K2Node_BangoSleep.generated.h"

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
	
	UPROPERTY(EditAnywhere, DisplayName = "Exec Skip")
	bool bEnableSkipExecPin;
	
	UPROPERTY(EditAnywhere, DisplayName = "Exec Cancel")
	bool bEnableCancelExecPin;

	UPROPERTY(EditAnywhere, DisplayName = "Conditional Skip")
	bool bEnableSkipConditionPin;
	
	UPROPERTY(EditAnywhere, DisplayName = "Conditional Cancel")
	bool bEnableCancelConditionPin;

public:
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void AllocateDefaultPins() override;

	void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;	
	
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