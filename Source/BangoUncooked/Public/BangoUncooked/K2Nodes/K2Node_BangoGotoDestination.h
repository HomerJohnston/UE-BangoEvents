#pragma once

#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "BangoUncooked/NodeBuilder/BangoNodeBuilder.h"

#include "K2Node_BangoGotoDestination.generated.h"

using namespace BangoNodeBuilder;

UCLASS(MinimalAPI, DisplayName = "Goto (Destination)")
class UK2Node_BangoGotoDestination : public UK2Node_BangoBase
{
	GENERATED_BODY()
	
	friend class UK2Node_BangoGotoSource;
	
public:
	UK2Node_BangoGotoDestination();
	
protected:
	UPROPERTY(EditAnywhere)
	FName Name;
	
public:
	FName GetRerouteName() const { return Name; }
	
	bool IsNodeRootSet() const override { return true; }
public:
	void AllocateDefaultPins() override;
	
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	
	UEdGraphPin* ExpandedExecPin = nullptr;
	
	UEdGraphPin* GetExpandedExecPin();
	
	class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
};

MAKE_NODE_TYPE(BangoGotoDestination, UK2Node_BangoGotoDestination, NORMAL_CONSTRUCTION, Exec, Then);	

void BangoGotoDestination::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
}
