#include "BangoGraphPanelNodeFactory.h"
#include "K2Node_CallFunction.h"
#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "Nodes/SGraphNode_BangoSleep.h"

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Bango::CreateNode(class UEdGraphNode* Node) const
{
    auto* FunctionNode = Cast<UK2Node_CallFunction>(Node);

    if (FunctionNode && FunctionNode->FunctionReference.GetMemberName() == FName("Sleep"))
    {
        return SNew(SGraphNode_BangoSleep, FunctionNode);
    }

    return nullptr;
}
