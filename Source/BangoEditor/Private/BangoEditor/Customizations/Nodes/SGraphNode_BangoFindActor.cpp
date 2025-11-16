#include "SGraphNode_BangoFindActor.h"

#include "SGraphPin.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"

void SGraphNode_BangoFindActor::Construct(const FArguments& InArgs, class UEdGraphNode* InNode)
{
	GraphNode = InNode;
	
	UpdateGraphNode();
}

void SGraphNode_BangoFindActor::CreatePinWidgets()
{
	SGraphNodeK2Base::CreatePinWidgets();
}

TSharedPtr<SGraphPin> SGraphNode_BangoFindActor::CreatePinWidget(UEdGraphPin* Pin) const
{
	TSharedPtr<SGraphPin> PinWidget = SGraphNodeK2Base::CreatePinWidget(Pin);
	
	PinWidget->SetShowLabel(false);
	
	return PinWidget;
}

void SGraphNode_BangoFindActor::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	SGraphNodeK2Base::AddPin(PinToAdd);
}

TArray<FOverlayWidgetInfo> SGraphNode_BangoFindActor::GetOverlayWidgets(bool bSelected, const FVector2f& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets = SGraphNode::GetOverlayWidgets(bSelected, WidgetSize);
	
	TSubclassOf<AActor> Subclass = GetBangoFindActorNode()->GetCastTo();
	
	if (Subclass)
	{
		FString Label = Subclass->GetName();
		
		const int32 MaxChars = 15;
		const FString Ellipsis = "...";
		
		if (Label.Len() > MaxChars + 3)
		Label = Label.Left(MaxChars) + Ellipsis;
		
		FOverlayWidgetInfo& CastInfo = Widgets.Add_GetRef(FOverlayWidgetInfo());
		
		CastInfo.Widget = SNew(SBorder)
		.BorderImage(nullptr)
		.Padding(0)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.ViaCast")))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Label))
			]
		];
		
		CastInfo.OverlayOffset = FVector2f((WidgetSize.X + 6), 14);
	}
	
	return Widgets;
}

UK2Node_BangoFindActor* SGraphNode_BangoFindActor::GetBangoFindActorNode() const
{
	return Cast<UK2Node_BangoFindActor>(GetNodeObj());
}
