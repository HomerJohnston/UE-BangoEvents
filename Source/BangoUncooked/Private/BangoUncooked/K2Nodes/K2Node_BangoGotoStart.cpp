#include "BangoUncooked/K2Nodes/K2Node_BangoGotoStart.h"

#include "BangoUncooked/NodeBuilder/BangoNodeBuilder.h"
#include "BangoUncooked/Subsystems/BangoNamedReroutesHelper.h"

using namespace BangoNodeBuilder;

#define LOCTEXT_NAMESPACE "BangoUncooked";

UK2Node_BangoGotoStart::UK2Node_BangoGotoStart()
{
	bShowNodeProperties = true;
}

void UK2Node_BangoGotoStart::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	
	if (bShowOutExecPin)
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	}
}

FText UK2Node_BangoGotoStart::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return INVTEXT("Goto");
}

void UK2Node_BangoGotoStart::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	Super::ExpandNode(Compiler, SourceGraph);

	const UEdGraphSchema_K2* Schema = Compiler.GetSchema();
	bool bIsErrorFree = true;

	namespace NB = BangoNodeBuilder;
	NB::Builder Builder(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(5, 5));
	
	// -----------------
	// Make nodes
	
	auto Node_This =					Builder.WrapExistingNode<NB::BangoGotoSource>(this);
	auto Node_Sequence =				Builder.MakeNode<NB::ExecutionSequence>(2, 0);

	// -----------------
	// Post-setup

	FString UniqueID = *Compiler.GetGuid(this);
	
	Builder.FinishDeferredNodes();
	
	// -----------------
	// Make connections

	// First input
	Builder.CopyExternalConnection(Node_This.Exec, Node_Sequence.Exec);

	if (bShowOutExecPin)
	{
		if (Node_This.Then->HasAnyConnections())
		{
			Builder.CopyExternalConnection(Node_This.Then, Node_Sequence->GetThenPinGivenIndex(ConnectionsMade++));
		}	
	}
	
	for (TObjectPtr<class UEdGraphNode> Node : SourceGraph->Nodes)
	{
		
		if (UK2Node_BangoGotoDestination* DestinationNode = Cast<UK2Node_BangoGotoDestination>(Node))
		{
			if (DestinationNode->GetRerouteName() == this->GetRerouteName())
			{
				while (!Node_Sequence->GetThenPinGivenIndex(ConnectionsMade))
				{
					Node_Sequence->AddInputPin();
				}
				
				Builder.CreateConnection(Node_Sequence->GetThenPinGivenIndex(ConnectionsMade++), DestinationNode->GetExpandedExecPin());
			}
		} 
	}
	
	// Done!
	if (!bIsErrorFree)
	{
		// TODO why the fuck won't this compile?
		//Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

void UK2Node_BangoGotoStart::ConnectToDestination(UEdGraphPin* Destination)
{
	// We can't actually make connections from the far side because this node has not been expanded yet. Stash the things we're going to connect to so we can do it in our own expansion.
	RequestedDestinations.Add(Destination);
}

#undef LOCTEXT_NAMESPACE