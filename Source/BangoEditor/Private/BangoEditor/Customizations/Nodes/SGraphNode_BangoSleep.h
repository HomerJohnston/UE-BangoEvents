#pragma once

#include "SGraphNode.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class SGraphNode_BangoSleep : public SGraphNodeK2Base
{
public:
    SLATE_BEGIN_ARGS(SGraphNode_BangoSleep)
    {
        
    }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, class UEdGraphNode* InNode);

    void CreatePinWidgets() override;

    TOptional<float> ProgressBar_Percent() const;
    void UpdateGraphNode() override;

    TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2f& WidgetSize) const override;
    
    TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;

    void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

    TSharedPtr<SVerticalBox> InnerVerticalBox;

    void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
    
    void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};
