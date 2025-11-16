#pragma once
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#include "_K2NodeBangoBase.generated.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UCLASS(Abstract, MinimalAPI)
class UK2NodeBangoBase : public UK2Node
{
	GENERATED_BODY()

protected:
	bool bIsLatent = false;
	bool bHideLatentIcon = false;
	bool bShowShowNodeProperties = false;
	
public:
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	
	
	FText GetMenuCategory() const override;
	
	bool IsLatentForMacros() const override;

	FName GetCornerIcon() const override;
	
	FText GetToolTipHeading() const override;

	bool ShouldShowNodeProperties() const override;
	
	FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	
	FLinearColor GetNodeTitleColor() const override;
};

#undef LOCTEXT_NAMESPACE
