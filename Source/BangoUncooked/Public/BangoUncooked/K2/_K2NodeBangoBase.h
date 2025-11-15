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
};

#undef LOCTEXT_NAMESPACE

// Create a node
#define __INTERNAL__MAKE_NODE_BASE(NodeName, NodeType, PosX, PosY)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<NodeType>(this, SourceGraph);\
	Node_##NodeName->SetPosition(FVector2f(GRAPH_ANCHOR.X + PosX, GRAPH_ANCHOR.Y + PosY));\

#define MAKE_BASIC_NODE_OLD(NodeName, NodeType, PosX, PosY)\
	__INTERNAL__MAKE_NODE_BASE(NodeName, NodeType, PosX, PosY);\
	Node_##NodeName->AllocateDefaultPins();\

#define MAKE_SELFNODE_AND_PINS(PosX, PosY)\
	__INTERNAL__MAKE_NODE_BASE(Self, UK2Node_Self, PosX, PosY)\
	Node_Self->AllocateDefaultPins();\
	UEdGraphPin* Node_Self_Get = Node_Self->FindPin(UEdGraphSchema_K2::PN_Self)\

// Note that it may be necessary to call AllocateDefaultPins again later, if you hook up dynamic inputs
#define MAKE_FUNCNODE_OLD(NodeName, Class, Function, PosX, PosY)\
	__INTERNAL__MAKE_NODE_BASE(NodeName, UK2Node_CallFunction, PosX, PosY);\
	Node_##NodeName->SetFromFunction(Class::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(Class, Function)));\
	Node_##NodeName->AllocateDefaultPins()\

#define MAKE_DELEGATE_NODE(NodeName, Class, Function, PosX, PosY)\
	__INTERNAL__MAKE_NODE_BASE(NodeName, UK2Node_AddDelegate);\
	Node_##NodeName->SetFromProperty(Class->FindPropertyByName(#Function), false, Class);\
	Node_##NodeName->AllocateDefaultPins();\

/*
#define MAKE_PROPERTY_GETTER_NODE_AND_PINS(NodeName, PropertyName)\
	auto* Node_##NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_VariableGet>(this, SourceGraph);\
	Node_##NodeName->VariableReference.SetExternalMember(FName(#PropertyName), StaticClass());\
	Node_##NodeName->AllocateDefaultPins();\
	UEdGraphPin* Node_##NodeName##_Get = Node_##NodeName->GetValuePin();\
	UEdGraphPin* Node_##NodeName##_Self = Node_##NodeName->FindPin(UEdGraphSchema_K2::PN_Self);\
	//CompilerContext.MovePinLinksToIntermediate(*FindPin(UEdGraphSchema_K2::PN_Self), *Node_##NodeName##_Self);\
*/

/*
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
*/


#define GET_PIN(NodeName, PinName)\
	UEdGraphPin* Node_##NodeName##_##PinName = Node_##NodeName->Get##PinName##Pin()

#define FIND_PIN(NodeName, PinName)\
	UEdGraphPin* Node_##NodeName##_##PinName = Node_##NodeName->FindPin(TEXT(#PinName))

#define DECLARE_PINS(...)\
	UEdGraphPin* __VA_ARGS__\

/*
// Count the number of arguments (up to 10 for simplicity)
#define PP_NARG(...) PP_NARG_(__VA_ARGS__, PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define PP_RSEQ_N() 10,9,8,7,6,5,4,3,2,1,0

// Concatenate tokens
#define PP_CAT(a, b) PP_CAT_(a, b)
#define PP_CAT_(a, b) a##b

// Define one level of expansion for each possible argument count
#define DECLARE_MEMBERS_1(a1) UEdGraphPin* a1;
#define DECLARE_MEMBERS_2(a1, a2) UEdGraphPin* a1; UEdGraphPin* a2;
#define DECLARE_MEMBERS_3(a1, a2, a3) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3;
#define DECLARE_MEMBERS_4(a1, a2, a3, a4) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4;
#define DECLARE_MEMBERS_5(a1, a2, a3, a4, a5) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4; UEdGraphPin* a5;
#define DECLARE_MEMBERS_6(a1, a2, a3, a4, a5, a6) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4; UEdGraphPin* a5; UEdGraphPin* a6;
#define DECLARE_MEMBERS_7(a1, a2, a3, a4, a5, a6, a7) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4; UEdGraphPin* a5; UEdGraphPin* a6; UEdGraphPin* a7;
#define DECLARE_MEMBERS_8(a1, a2, a3, a4, a5, a6, a7, a8) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4; UEdGraphPin* a5; UEdGraphPin* a6; UEdGraphPin* a7; UEdGraphPin* a8;
#define DECLARE_MEMBERS_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4; UEdGraphPin* a5; UEdGraphPin* a6; UEdGraphPin* a7; UEdGraphPin* a8; UEdGraphPin* a9;
#define DECLARE_MEMBERS_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) UEdGraphPin* a1; UEdGraphPin* a2; UEdGraphPin* a3; UEdGraphPin* a4; UEdGraphPin* a5; UEdGraphPin* a6; UEdGraphPin* a7; UEdGraphPin* a8; UEdGraphPin* a9; UEdGraphPin* a10;

// Dispatcher macro
#define DECLARE_MEMBERS(...) \
	PP_CAT(DECLARE_MEMBERS_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)\

// ---- Struct declaration ----
#define DECLARE_STRUCT(Name, NodeType ...) \
	namespace Node\
	{\
		NodeType Node;\
		struct Name\
		{\
			\
			DECLARE_MEMBERS(__VA_ARGS__) \
		};
	}\

template<typename T>
struct NodeWrapper
{
	T Node;
	
	// Different instances of NodeWrapper will have different counts and names of pins, this is just one example.
	// All pins will be UEdGraphPin* types.
	UEdGraphPin* Pin1;
	UEdGraphPin* Pin2;
	UEdGraphPin* Pin3;
	UEdGraphPin* Pin4;
};
*/
