#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"

#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UK2Node_BangoFindActor::UK2Node_BangoFindActor()
{
}

void UK2Node_BangoFindActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UK2Node_BangoFindActor::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FName("ActorID"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, FName("FoundActor"));
	
}

FText UK2Node_BangoFindActor::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return Super::GetNodeTitle(TitleType);
}

void UK2Node_BangoFindActor::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	Super::ExpandNode(Compiler, SourceGraph);

	const UEdGraphSchema_K2* Schema = Compiler.GetSchema();
	bool bIsErrorFree = true;

	namespace NB = Bango_NodeBuilder;
	NB::Builder Builder(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(5, 5));
	
	// -----------------
	// Make nodes
	
	auto Node_This =					Builder.WrapExistingNode<NB::BangoFindActor>(this);
	auto Node_FindActorFunction	=		Builder.MakeNode<NB::CallFunction>(0, 1);
	// -----------------
	// Post-setup

	FString UniqueID = *Compiler.GetGuid(this);
	
	Node_FindActorFunction->SetFromFunction(UBangoActorIDBlueprintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoActorIDBlueprintFunctionLibrary, GetActor)));
	
	Builder.FinishDeferredNodes();
	
	// -----------------
	// Make connections

	// First input
	Builder.CopyExternalConnection(Node_This.ActorID, Node_FindActorFunction.Exec);

	// Final output
	// Builder.CopyExternalConnection(Node_This.Completed, ???.Then);
	
	// Done!
	if (!bIsErrorFree)
	{
		Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "K2Node_LoadAsset: Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE