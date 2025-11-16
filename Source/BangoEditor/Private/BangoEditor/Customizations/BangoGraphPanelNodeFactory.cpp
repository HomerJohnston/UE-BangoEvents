#include "BangoGraphPanelNodeFactory.h"
#include "K2Node_CallFunction.h"
#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoRunScript.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoSleep.h"
#include "Nodes/SGraphNode_BangoSleep.h"
#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"
#include "Nodes/SGraphNode_BangoFindActor.h"

#define TRY_MAKE_WIDGET(NodeToCheck, K2NodeType, SlateWidgetType)\
    if (NodeToCheck->IsA<K2NodeType>())\
    {\
        return SNew(SlateWidgetType, Node);\
    }\

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Bango::CreateNode(class UEdGraphNode* Node) const
{
    if (!Node->IsA<UK2NodeBangoBase>())
    {
        return nullptr;
    }

    TRY_MAKE_WIDGET(Node, UK2Node_BangoFindActor, SGraphNode_BangoFindActor);
    TRY_MAKE_WIDGET(Node, UK2Node_BangoSleep, SGraphNode_BangoSleep);

    return nullptr;
}
