#include "BangoGraphPanelNodeFactory.h"
#include "K2Node_CallFunction.h"
#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoRunScript.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoSleep.h"
#include "Nodes/SGraphNode_BangoSleep.h"
#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Bango::CreateNode(class UEdGraphNode* Node) const
{
    if (Node->IsA<UK2NodeBangoBase>())
    {
        if (Node->IsA<UK2Node_BangoRunScript>())
        {
            //
        } else
        if (Node->IsA<UK2Node_BangoSleep>())
        {
            return SNew(SGraphNode_BangoSleep, Node);
        }
    }
    /*
    auto* FunctionNode = Cast<UK2Node_CallFunction>(Node);

    // TODO I need to make sure this ONLY applies to the BangoScriptObject func!
    // TODO I can't do this??? I'll just make a stupid K2Node
    if (FunctionNode && FunctionNode->FunctionReference.GetMemberName() == FName("Sleep"))
    {
        return SNew(SGraphNode_BangoSleep, FunctionNode);
    }
    */
    return nullptr;
}
