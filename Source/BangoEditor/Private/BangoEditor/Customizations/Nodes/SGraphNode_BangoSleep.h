#pragma once

#include "SGraphNode.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class FBangoSleepAction;

class SGraphNode_BangoSleep : public SGraphNodeK2Base
{
public:
    SLATE_BEGIN_ARGS(SGraphNode_BangoSleep)
    {
    }
    SLATE_END_ARGS()

    float AbortTime = -1.0f;
    FBangoSleepAction* CurrentSleepAction = nullptr;
    
    void Construct(const FArguments& InArgs, class UEdGraphNode* InNode);

    void CreatePinWidgets() override;

    TOptional<float> GetProgressBarPercent() const;

    void UpdateGraphNode() override;
    
    TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2f& WidgetSize) const override;
    
    TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;

    void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

    TSharedPtr<SVerticalBox> InnerVerticalBox;

    void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
    
    EVisibility Visibility_ProgressBar() const;
    
    FSlateColor ColorAndOpacity_Hourglass() const;

    FBangoSleepAction* GetSleepAction() const;

    void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    void OnAborted();
};
