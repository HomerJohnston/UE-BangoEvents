#include "BangoGraphPanelNodeFactory.h"
#include "K2Node_CallFunction.h"
#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoStart.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoDestination.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoRunScript.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoSleep.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoThis.h"
#include "Nodes/SGraphNode_BangoSleep.h"
#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"
#include "Nodes/SGraphNode_BangoFindActor.h"
#include "Nodes/SGraphNode_BangoGotoDestination.h"
#include "Nodes/SGraphNode_BangoGotoStart.h"
#include "Nodes/SGraphNode_BangoThis.h"

#define TRY_MAKE_WIDGET(NodeToCheck, K2NodeType, SlateWidgetType)\
    if (NodeToCheck->IsA<K2NodeType>())\
    {\
        return SNew(SlateWidgetType, Node);\
    }\

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Bango::CreateNode(class UEdGraphNode* Node) const
{
    if (!Node->IsA<UK2Node_BangoBase>())
    {
        return nullptr;
    }

    TRY_MAKE_WIDGET(Node, UK2Node_BangoFindActor, SGraphNode_BangoFindActor);
    TRY_MAKE_WIDGET(Node, UK2Node_BangoSleep, SGraphNode_BangoSleep);
    TRY_MAKE_WIDGET(Node, UK2Node_BangoGotoStart, SGraphNode_BangoGotoStart);
    TRY_MAKE_WIDGET(Node, UK2Node_BangoGotoDestination, SGraphNode_BangoGotoDestination);
	TRY_MAKE_WIDGET(Node, UK2Node_BangoThis, SGraphNode_BangoThis);
    return nullptr;
}
