#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"

#include "K2Node_DynamicCast.h"
#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UK2Node_BangoFindActor::UK2Node_BangoFindActor()
{
	bShowShowNodeProperties = true;
}

void UK2Node_BangoFindActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.MemberProperty == StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ThisClass, CastTo)))
	{
		ReconstructNode();
	}
}

void UK2Node_BangoFindActor::AllocateDefaultPins()
{
	auto* ActorIDPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FName("ActorID"));
	auto* FoundActorPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, FName("FoundActor"));
	
	if (IsValid(CastTo))
	{
		FoundActorPin->PinType.PinSubCategoryObject = CastTo;
	}
	else
	{
		FoundActorPin->PinType.PinSubCategoryObject = AActor::StaticClass();		
	}
	
	ActorIDPin->PinFriendlyName = LOCTEXT("BangoFindActorNode_ActorIDPinLabel", " ");
	FoundActorPin->PinFriendlyName = LOCTEXT("BangoFindActorNode_FoundActorLabel", " ");
}

FText UK2Node_BangoFindActor::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("BangoFindActorNode_Title", "Find Actor");
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
	auto Node_CastToType =				Builder.MakeNode<NB::DynamicCast_Pure>(1, 1);
	
	// -----------------
	// Post-setup

	FString UniqueID = *Compiler.GetGuid(this);
	
	Node_FindActorFunction->SetFromFunction(UBangoActorIDBlueprintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoActorIDBlueprintFunctionLibrary, GetActor)));
	
	if (IsValid(CastTo))
	{
		Node_CastToType->TargetType = CastTo;
	}
	
	Builder.FinishDeferredNodes();
	
	// -----------------
	// Make connections

	// First input
	Builder.CopyExternalConnection(Node_This.ActorID, Node_FindActorFunction.FindPin("ActorID"));

	if (IsValid(CastTo))
	{
		Builder.CreateConnection(Node_FindActorFunction->GetReturnValuePin(), Node_CastToType.ObjectToCast);
		Builder.CopyExternalConnection(Node_This.FoundActor, Node_CastToType.CastedObject);
	}
	else
	{
		Builder.CopyExternalConnection(Node_This.FoundActor, Node_FindActorFunction->GetReturnValuePin());
	}
	
	// Final output
	Builder.CopyExternalConnection(Node_This.FoundActor, Node_CastToType.CastedObject);
	
	// Done!
	if (!bIsErrorFree)
	{
		Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "K2Node_LoadAsset: Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE