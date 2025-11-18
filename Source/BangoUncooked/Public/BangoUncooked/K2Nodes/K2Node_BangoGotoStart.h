#pragma once

#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "BangoUncooked/Private/BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"

#include "K2Node_BangoGotoStart.generated.h"


UCLASS(MinimalAPI, DisplayName = "Goto (Source)")
class UK2Node_BangoGotoStart : public UK2Node_BangoBase
{
	GENERATED_BODY()

	friend class UK2Node_BangoGotoDestination;
	
public:
	UK2Node_BangoGotoStart();
	
protected:
	UPROPERTY(EditAnywhere)
	FName Name;
	
	UPROPERTY(EditAnywhere)
	bool bShowOutExecPin = false;
	
	int32 ConnectionsMade;
	
	TArray<UEdGraphPin*> RequestedDestinations;
	
public:
	FName GetRerouteName() const { return Name; }
	
public:
	void AllocateDefaultPins() override;
	
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;
};

using namespace BangoNodeBuilder;

MAKE_NODE_TYPE(BangoGotoSource, UK2Node_BangoGotoStart, NORMAL_CONSTRUCTION, Exec, Then);

// BangoNodeBuilder Wrapper
inline void BangoGotoSource::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
}