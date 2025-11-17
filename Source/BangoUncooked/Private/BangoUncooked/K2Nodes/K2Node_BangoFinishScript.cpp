#include "BangoUncooked/K2Nodes/K2Node_BangoFinishScript.h"

#include "BangoUncooked/K2Nodes/Base/_BangoMenuSubcategories.h"

#define LOCTEXT_NAMESPACE "BangoUncooked"

UK2Node_BangoFinishScript::UK2Node_BangoFinishScript()
{
	MenuSubcategory = BangoSubcategories::Scripting;
}

void UK2Node_BangoFinishScript::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
}

FText UK2Node_BangoFinishScript::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("K2Node_BangoFinishScript_NodeTitle", "Finish Script");
}

void UK2Node_BangoFinishScript::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	Super::ExpandNode(Compiler, SourceGraph);
}

#undef LOCTEXT_NAMESPACE