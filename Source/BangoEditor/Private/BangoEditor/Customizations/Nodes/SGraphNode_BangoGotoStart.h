#pragma once

#include "SGraphNode.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class UK2Node_BangoGotoStart;
class FBangoGotoStartAction;

class SGraphNode_BangoGotoStart : public SGraphNodeK2Base
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_BangoGotoStart)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UEdGraphNode* InNode);

	TSharedPtr<SVerticalBox> InnerVerticalBox;
	
	void UpdateGraphNode() override;

	TSharedRef<SWidget> CreateNodeContentArea() override;
	
	TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2f& WidgetSize) const override;
    
	UK2Node_BangoGotoStart* GetGotoStartNode() const;
	
	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
