#pragma once

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
};

#undef LOCTEXT_NAMESPACE

// Create a node
#define MAKE_NODE_SELF()\
	auto* Node_Self = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);\
	Node_Self->AllocateDefaultPins();\
	UEdGraphPin* Node_Self_Get = Node_Self->FindPin(UEdGraphSchema_K2::PN_Self);\
	
#define MAKE_NODE(NodeName, NodeType, PosX, PosY)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<NodeType>(this, SourceGraph);\
	Node_##NodeName->SetPosition(FVector2f(GraphAnchor.X + PosX, GraphAnchor.Y + PosY))\

#define MAKE_NODE_FUNC(NodeName, Class, Function, PosX, PosY)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);\
	Node_##NodeName->SetFromFunction(Class::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(Class, Function)));\
	Node_##NodeName->SetPosition(FVector2f(GraphAnchor.X + PosX, GraphAnchor.Y + PosY))\

#define MAKE_DELEGATE_NODE(NodeName, PosX, PosY)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_AddDelegate>(this, SourceGraph);\
	/* Node_##NodeName->SetFromProperty();*/\
	Node_##NodeName->SetPosition(FVector2f(GraphAnchor.X + PosX, GraphAnchor.Y + PosY))\

#define MAKE_PROPERTY_GETTER_NODE_AND_PINS(NodeName, PropertyName)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_VariableGet>(this, SourceGraph);\
	Node_##NodeName->VariableReference.SetExternalMember(FName(#PropertyName), StaticClass());\
	Node_##NodeName->AllocateDefaultPins();\
	UEdGraphPin* Node_##NodeName##_Get = Node_##NodeName->GetValuePin();\
	UEdGraphPin* Node_##NodeName##_Self = Node_##NodeName->FindPin(UEdGraphSchema_K2::PN_Self);\
	//CompilerContext.MovePinLinksToIntermediate(*FindPin(UEdGraphSchema_K2::PN_Self), *Node_##NodeName##_Self);\

#define MAKE_PROPERTY_SETTER_NODE_AND_PINS(NodeName, PropertyName)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_VariableSet>(this, SourceGraph);\
	Node_##NodeName->VariableReference.SetExternalMember(FName(#PropertyName), StaticClass());\
	Node_##NodeName->AllocateDefaultPins();\
	UEdGraphPin* Node_##NodeName##_Exec = Node_##NodeName->GetExecPin();\
	UEdGraphPin* Node_##NodeName##_Then = Node_##NodeName->GetThenPin();\
	UEdGraphPin* Node_##NodeName##_Value = Node_##NodeName->FindPin(FName(#PropertyName));\
	UEdGraphPin* Node_##NodeName##_Output_Get = Node_##NodeName->FindPin(FName("Output_Get"));\
	UEdGraphPin* Node_##NodeName##_Self = Node_##NodeName->FindPin(UEdGraphSchema_K2::PN_Self);\
	//CompilerContext.MovePinLinksToIntermediate(*Node_Self_Get, *Node_##NodeName##_Self);\

#define GET_PIN(NodeName, VarName, PinName)\
	UEdGraphPin* Node_##NodeName##_##VarName = Node_##NodeName->FindPin(PinName)\
	