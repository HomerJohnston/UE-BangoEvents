#include "SGraphNode_BangoGotoStart.h"

#include "IDocumentation.h"
#include "SCommentBubble.h"
#include "SGraphNode_BangoGotoDestination.h"
#include "SGraphPanel.h"
#include "SLevelOfDetailBranchNode.h"
#include "TutorialMetaData.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoDestination.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoStart.h"
#include "Engine/Font.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/SToolTip.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

void SGraphNode_BangoGotoStart::Construct(const FArguments& InArgs, class UEdGraphNode* InNode)
{
    GraphNode = InNode;
	TitleBorderMargin.Right = TitleBorderMargin.Left; // By default, big right padding to grow the node out. I don't want that.

    UpdateGraphNode();
}

void SGraphNode_BangoGotoStart::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	
	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	//
	//             ______________________
	//            |      TITLE AREA      |
	//            +-------+------+-------+
	//            | (>) L |      | R (>) |
	//            | (>) E |      | I (>) |
	//            | (>) F |      | G (>) |
	//            | (>) T |      | H (>) |
	//            |       |      | T (>) |
	//            |_______|______|_______|
	//
	TSharedPtr<SVerticalBox> MainVerticalBox;
	SetupErrorReporting();

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	// Get node icon
	IconColor = FLinearColor::White;
	const FSlateBrush* IconBrush = nullptr;
	if (GraphNode != NULL && GraphNode->ShowPaletteIconOnNode())
	{
		IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();
	}

	TSharedRef<SOverlay> DefaultTitleAreaWidget =
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.Image( FAppStyle::GetBrush("Graph.Node.TitleGloss") )
			.ColorAndOpacity( this, &SGraphNode::GetNodeTitleIconColor )
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage( FAppStyle::GetBrush("Graph.Node.ColorSpill") )
				.Padding(TitleBorderMargin)
				.BorderBackgroundColor( this, &SGraphNode::GetNodeTitleColor )
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Top)
					.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
					.AutoWidth()
					[
						SNew(SImage)
						.Image(IconBrush)
						.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							CreateTitleWidget(NodeTitle)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							NodeTitle.ToSharedRef()
						]
					]
				]
			]
			/*
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(0, 0, 5, 0)
			.AutoWidth()
			[
				CreateTitleRightWidget()
			]
			*/
		]
		+SOverlay::Slot()
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Visibility(EVisibility::HitTestInvisible)			
			.BorderImage( FAppStyle::GetBrush( "Graph.Node.TitleHighlight" ) )
			.BorderBackgroundColor( this, &SGraphNode::GetNodeTitleIconColor )
			[
				SNew(SSpacer)
				.Size(FVector2D(20,20))
			]
		];

	SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

	auto UseLowDetailNodeTitle = [this] () -> bool
	{
		if (InlineEditableText.IsValid())
		{
			if (const SGraphPanel* MyOwnerPanel = GetOwnerPanel().Get())
			{
				return (MyOwnerPanel->GetCurrentLOD() <= EGraphRenderingLOD::LowestDetail) && !InlineEditableText->IsInEditMode();
			}
		}

		return false;
	};
	
	TSharedPtr<SLevelOfDetailBranchNode> __TitleLODBranchNode;
	SAssignNew(__TitleLODBranchNode, SLevelOfDetailBranchNode)
	.UseLowDetailSlot_Lambda(UseLowDetailNodeTitle)
	.LowDetail()
	[
		SNew(SBorder)
		.BorderImage( FAppStyle::GetBrush("Graph.Node.ColorSpill") )
		.Padding( FMargin(75.0f, 22.0f) ) // Saving enough space for a 'typical' title so the transition isn't quite so abrupt
		.BorderBackgroundColor( this, &SGraphNode::GetNodeTitleColor )
	]
	.HighDetail()
	[
		DefaultTitleAreaWidget
	];

	if (!SWidget::GetToolTip().IsValid())
	{
		TSharedRef<SToolTip> DefaultToolTip = IDocumentation::Get()->CreateToolTip( TAttribute< FText >( this, &SGraphNode::GetNodeTooltip ), NULL, GraphNode->GetDocumentationLink(), GraphNode->GetDocumentationExcerptName() );
		SetToolTip(DefaultToolTip);
	}
	
	// Setup a meta tag for this node
	FGraphNodeMetaData TagMeta(TEXT("Graphnode"));
	PopulateMetaTag(&TagMeta);
	
	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );

	InnerVerticalBox = SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			__TitleLODBranchNode.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			CreateNodeContentArea()
		];

	TSharedPtr<SWidget> EnabledStateWidget = GetEnabledStateWidget();
	if (EnabledStateWidget.IsValid())
	{
		InnerVerticalBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.Padding(FMargin(2, 0))
			[
				EnabledStateWidget.ToSharedRef()
			];
	}

	InnerVerticalBox->AddSlot()
		.AutoHeight()
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			ErrorReporting->AsWidget()
		];

	InnerVerticalBox->AddSlot()
		.AutoHeight()
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			VisualWarningReporting->AsWidget()
		];


	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(MainVerticalBox, SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SOverlay)
				.AddMetaData<FGraphNodeMetaData>(TagMeta)
				+SOverlay::Slot()
				.Padding(Settings->GetNonPinNodeBodyPadding())
				[
					SNew(SImage)
					.Image(GetNodeBodyBrush())
					.ColorAndOpacity(this, &SGraphNode::GetNodeBodyColor)
				]
				+SOverlay::Slot()
				[
					InnerVerticalBox.ToSharedRef()
				]
			]			
		];

	bool SupportsBubble = true;
	if (GraphNode != nullptr)
	{
		SupportsBubble = GraphNode->SupportsCommentBubble();
	}

	if (SupportsBubble)
	{
		// Create comment bubble
		TSharedPtr<SCommentBubble> CommentBubble;
		const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

		SAssignNew(CommentBubble, SCommentBubble)
			.GraphNode(GraphNode)
			.Text(this, &SGraphNode::GetNodeComment)
			.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
			.OnToggled(this, &SGraphNode::OnCommentBubbleToggled)
			.ColorAndOpacity(CommentColor)
			.AllowPinning(true)
			.EnableTitleBarBubble(true)
			.EnableBubbleCtrls(true)
			.GraphLOD(this, &SGraphNode::GetCurrentLOD)
			.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

		GetOrAddSlot(ENodeZone::TopCenter)
			.SlotOffset2f(TAttribute<FVector2f>(CommentBubble.Get(), &SCommentBubble::GetOffset2f))
			.SlotSize2f(TAttribute<FVector2f>(CommentBubble.Get(), &SCommentBubble::GetSize2f))
			.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
			.VAlign(VAlign_Top)
			[
				CommentBubble.ToSharedRef()
			];
	}

	CreateBelowWidgetControls(MainVerticalBox);
	
	/*
	InnerVerticalBox->AddSlot()
	[
		SNew(SBox)
		.HeightOverride(4)
		[
			SNew(SProgressBar)
			.Visibility(this, &SGraphNode_BangoGotoStart::Visibility_ProgressBar)
			.BarFillStyle(EProgressBarFillStyle::Mask)
			.BarFillType(EProgressBarFillType::FillFromCenterHorizontal)
			.Percent(this, &SGraphNode_BangoGotoStart::GetProgressBarPercent)
		]
	];
	*/
	
	CreatePinWidgets();
	CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

TSharedRef<SWidget> SGraphNode_BangoGotoStart::CreateNodeContentArea()
{
	// NODE CONTENT AREA
	return SNew(SBorder)
		.BorderImage( FAppStyle::GetBrush("NoBorder") )
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding( FMargin(0,3) )
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				// LEFT
				SAssignNew(LeftNodeBox, SVerticalBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 12, 0)
			[
				SNew(SImage)
				.DesiredSizeOverride(FVector2D(20, 20))
				.Image(FBangoEditorStyle::GetImageBrush(BangoEditorBrushes.Icon_PortalIn))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				// RIGHT
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
}

TArray<FOverlayWidgetInfo> SGraphNode_BangoGotoStart::GetOverlayWidgets(bool bSelected, const FVector2f& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;

	FName PortalName = GetGotoStartNode()->GetRerouteName();
	
	if (PortalName != NAME_None)
	{
		FOverlayWidgetInfo& PortalNameWidget = Widgets.Add_GetRef(FOverlayWidgetInfo());
	
		PortalNameWidget.Widget = SNew(SBorder)
		.BorderImage(nullptr)
		.Padding(0)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SBorder)
			.Padding(8, 4)
			.BorderImage(FAppStyle::Get().GetBrush("FloatingBorder"))
			[
				SNew(STextBlock)
				.Text(FText::FromName(PortalName))
			]
		];
	
		PortalNameWidget.OverlayOffset = FVector2f((WidgetSize.X - 8), WidgetSize.Y * 0.5f);
	}
	
	return Widgets;
}

UK2Node_BangoGotoStart* SGraphNode_BangoGotoStart::GetGotoStartNode() const
{
	return Cast<UK2Node_BangoGotoStart>(GetNodeObj());
}

int32 SGraphNode_BangoGotoStart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (IsSelectedExclusively())
	{
		//if (!GEditor->IsPlaySessionInProgress())
		//{
			const auto& GraphChildren = GetOwnerPanel()->GetChildren();
						
			for (int32 i = 0; i < GraphChildren->Num(); ++i)
			{
				TSharedRef<SWidget> ChildWidget = GraphChildren->GetChildAt(i);
				
				if (ChildWidget->GetTypeAsString() == TEXT("SGraphNode_BangoGotoDestination"))
				{
					TSharedPtr<SGraphNode_BangoGotoDestination> Other = StaticCastSharedRef<SGraphNode_BangoGotoDestination>(ChildWidget);
					
					if (GetGotoStartNode()->GetRerouteName() == Other->GetGotoDestinationNode()->GetRerouteName())
					{
						TArray<FVector2f> Points;
						
						FVector2f Start = { AllottedGeometry.GetAbsoluteSize().X * 0.5f, AllottedGeometry.GetAbsoluteSize().Y * 0.5f };
						FVector2f End = Other->GetPosition2f() - this->GetPosition2f() + FVector2f(Other->GetPaintSpaceGeometry().GetAbsoluteSize().X * 0.5f, Other->GetPaintSpaceGeometry().GetAbsoluteSize().Y * 0.5f);
						
						FVector2f Dir = (End - Start).GetSafeNormal(30000.0f);
						
						if (Dir.IsZero())
						{
							continue;
						}
						
						Start = Start + 80.0f * Dir;
						End = End - 80.0f * Dir;
						
						FSlateDrawElement::MakeLines
						(
							OutDrawElements,
							LayerId++,
							AllottedGeometry.ToPaintGeometry(),
							{ Start, End },
							ESlateDrawEffect::None,
							FLinearColor::Green * FLinearColor::Gray
						);
					}
				}
			}	
		//}
	}
	
	return SGraphNodeK2Base::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

#undef LOCTEXT_NAMESPACE
