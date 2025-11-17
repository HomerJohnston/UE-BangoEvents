#pragma once

#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "K2Node_BangoFinishScript.generated.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UCLASS(MinimalAPI, DisplayName = "Finish")
class UK2Node_BangoFinishScript : public UK2Node_BangoBase
{
	GENERATED_BODY()

public:
	UK2Node_BangoFinishScript();
	
public:
	void AllocateDefaultPins() override;

	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;
	
	
};

#undef LOCTEXT_NAMESPACE