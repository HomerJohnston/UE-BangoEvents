#include "BangoUncooked/K2Nodes/K2Node_BangoGotoDestination.h"

#include "BangoUncooked/K2Nodes/K2Node_BangoGotoSource.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"
#include "BangoUncooked/Subsystems/BangoNamedReroutesHelper.h"

#define LOCTEXT_NAMESPACE "BangoUncooked"

UK2Node_BangoGotoDestination::UK2Node_BangoGotoDestination()
{
	bShowNodeProperties = true;
}

void UK2Node_BangoGotoDestination::AllocateDefaultPins()
{
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	InputPin->bHidden = true;
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
}

FText UK2Node_BangoGotoDestination::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return INVTEXT("Goto (Destination)");
}

void UK2Node_BangoGotoDestination::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	Super::ExpandNode(Compiler, SourceGraph);

	const UEdGraphSchema_K2* Schema = Compiler.GetSchema();
	bool bIsErrorFree = true;

	namespace NB = BangoNodeBuilder;
	NB::Builder Builder(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(5, 5));
	
	// -----------------
	// Make nodes
	
	//using namespace Bango_NodeBuilder;
	auto Node_This =					Builder.WrapExistingNode<NB::BangoGotoDestination>(this);
	auto Node_Knot =					Builder.MakeNode<NB::Knot>(0, 0);
	
	// -----------------
	// Post-setup

	FString UniqueID = *Compiler.GetGuid(this);
	
	Builder.FinishDeferredNodes();
	
	// -----------------
	// Make connections

	if (!Node_This.Exec->HasAnyConnections())
	{
		// We haven't been wired up yet. Let's see if we can do it ourselves?
		
		for (TObjectPtr<class UEdGraphNode> Node : SourceGraph->Nodes)
		{
			if (UK2Node_BangoGotoSource* SourceNode = Cast<UK2Node_BangoGotoSource>(Node))
			{
				if (SourceNode->GetRerouteName() == this->GetRerouteName())
				{
					SourceNode->ConnectToDestination(Node_Knot.InputPin);
				}
			} 
		}
	}
	
	Builder.MoveExternalConnection(Node_This.Exec, Node_Knot.InputPin);
	Builder.MoveExternalConnection(Node_This.Then, Node_Knot.OutputPin);
	
	ExpandedExecPin = Node_Knot.InputPin;
	
	// Done!
	if (!bIsErrorFree)
	{
		Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

UEdGraphPin* UK2Node_BangoGotoDestination::GetExpandedExecPin()
{
	if (ExpandedExecPin)
	{
		return ExpandedExecPin;
	}
	
	return GetExecPin();
}

class FNodeHandlingFunctor* UK2Node_BangoGotoDestination::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FNodeHandlingFunctor(CompilerContext);
}

#undef LOCTEXT_NAMESPACE
