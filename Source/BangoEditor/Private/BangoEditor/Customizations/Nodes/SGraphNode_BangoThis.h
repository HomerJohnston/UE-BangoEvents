#pragma once

#include "SGraphNode.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class UK2Node_BangoThis;
class FBangoThisAction;

class SGraphNode_BangoThis : public SGraphNodeK2Base
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_BangoThis)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UEdGraphNode* InNode);

	TSharedRef<SWidget> CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle) override;
	
	void CreatePinWidgets() override;

	TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;

	void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	
	UK2Node_BangoThis* GetBangoThisNode() const;
	
	void UpdateGraphNode();

	void UpdateCompactNode();

	FText GetNodeCompactTitle_Impl() const;

	FSlateColor ColorAndOpacity_ActorLabel() const;
};
