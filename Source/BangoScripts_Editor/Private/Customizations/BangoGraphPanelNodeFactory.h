#pragma once

#include "EdGraphUtilities.h"

class FGraphPanelNodeFactory_Bango : public FGraphPanelNodeFactory
{
    TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* Node) const override;
};
