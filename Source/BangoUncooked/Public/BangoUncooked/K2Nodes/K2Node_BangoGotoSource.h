#pragma once

#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"

#include "K2Node_BangoGotoSource.generated.h"


UCLASS(MinimalAPI, DisplayName = "Goto (Source)")
class UK2Node_BangoGotoSource : public UK2Node_BangoBase
{
	GENERATED_BODY()

	friend class UK2Node_BangoGotoDestination;
	
public:
	UK2Node_BangoGotoSource();
	
protected:
	UPROPERTY(EditAnywhere)
	FName Name;
	
	int32 ConnectionsMade;
	
	TArray<UEdGraphPin*> RequestedDestinations;
	
public:
	FName GetRerouteName() const { return Name; }
	
public:
	void AllocateDefaultPins() override;
	
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;
	
	void ConnectToDestination(UEdGraphPin* Destination);
};

MAKE_NODE_TYPE(BangoGotoSource, UK2Node_BangoGotoSource, NORMAL_CONSTRUCTION, Exec, Then);
