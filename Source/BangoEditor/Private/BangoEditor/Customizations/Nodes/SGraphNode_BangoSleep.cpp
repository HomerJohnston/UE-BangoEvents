#include "BangoEditor/Customizations/Nodes/SGraphNode_BangoSleep.h"

#include "BangoKismetNodeInfoContext.h"
#include "BlueprintEditorSettings.h"
#include "GraphEditorSettings.h"
#include "IDocumentation.h"
#include "K2Node_TemporaryVariable.h"
#include "SCommentBubble.h"
#include "SGraphPanel.h"
#include "SGraphPin.h"
#include "TutorialMetaData.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Bango/Private/Bango/LatentActions/BangoSleepAction.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoSleep.h"
#include "Editor/KismetWidgets/Public/SLevelOfDetailBranchNode.h"
#include "KismetNodes/KismetNodeInfoContext.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

void SGraphNode_BangoSleep::Construct(const FArguments& InArgs, class UEdGraphNode* InNode)
{
    GraphNode = InNode;
	//IconColor = FLinearColor::Red;

    UpdateGraphNode();
}

void SGraphNode_BangoSleep::CreatePinWidgets()
{
    // Create Pin widgets for each of the pins.
    for (int32 PinIndex = 0; PinIndex < GraphNode->Pins.Num(); ++PinIndex)
    {
        UEdGraphPin* CurPin = GraphNode->Pins[PinIndex];

        if ( !ensureMsgf(CurPin->GetOuter() == GraphNode
            , TEXT("Graph node ('%s' - %s) has an invalid %s pin: '%s'; (with a bad %s outer: '%s'); skiping creation of a widget for this pin.")
            , *GraphNode->GetNodeTitle(ENodeTitleType::ListView).ToString()
            , *GraphNode->GetPathName()
            , (CurPin->Direction == EEdGraphPinDirection::EGPD_Input) ? TEXT("input") : TEXT("output")
            ,  CurPin->PinFriendlyName.IsEmpty() ? *CurPin->PinName.ToString() : *CurPin->PinFriendlyName.ToString()
            ,  CurPin->GetOuter() ? *CurPin->GetOuter()->GetClass()->GetName() : TEXT("UNKNOWN")
            ,  CurPin->GetOuter() ? *CurPin->GetOuter()->GetPathName() : TEXT("NULL")) )
        {
            continue;
        }

        CreateStandardPinWidget(CurPin);
    }
}

TOptional<float> SGraphNode_BangoSleep::GetProgressBarPercent() const
{
	FBangoSleepAction* SleepAction = GetSleepAction();

	if (SleepAction && SleepAction->Duration > KINDA_SMALL_NUMBER)
	{
		return SleepAction->TimeRemaining / SleepAction->Duration;
	}

	return 0.0f;
}

void SGraphNode_BangoSleep::UpdateGraphNode()
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
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(0, 0, 5, 0)
			.AutoWidth()
			[
				CreateTitleRightWidget()
			]
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
	
	InnerVerticalBox->AddSlot()
	[
		SNew(SBox)
		.HeightOverride(4)
		[
			SNew(SProgressBar)
			.Visibility(this, &SGraphNode_BangoSleep::Visibility_ProgressBar)
			.BarFillStyle(EProgressBarFillStyle::Mask)
			.BarFillType(EProgressBarFillType::FillFromCenterHorizontal)
			.Percent(this, &SGraphNode_BangoSleep::GetProgressBarPercent)
		]
	];
	
	CreatePinWidgets();
	CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

TArray<FOverlayWidgetInfo> SGraphNode_BangoSleep::GetOverlayWidgets(bool bSelected, const FVector2f& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;
	
	UK2Node_BangoSleep* Node = GetSleepNode();
	
	float ImageSize = 32.0f;
	
	FOverlayWidgetInfo Info;
	Info.OverlayOffset = FVector2f((WidgetSize.X / 2) - (ImageSize * 0.5f), 28);
	Info.Widget = SNew(SBorder)
		.BorderImage(nullptr)
		.Padding(0)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SImage)
			.Image(this, &SGraphNode_BangoSleep::Image_OverlayWidget)
			.ColorAndOpacity(this, &SGraphNode_BangoSleep::ColorAndOpacity_Hourglass)
			.DesiredSizeOverride(FVector2D(ImageSize, ImageSize))
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			.RenderTransform(this, &SGraphNode_BangoSleep::RenderTransform_Hourglass)
		];
	
	Widgets.Add(Info);

	return Widgets;
}

const FSlateBrush* SGraphNode_BangoSleep::Image_OverlayWidget() const
{
	UK2Node_BangoSleep* Node = GetSleepNode();
	
	const FSlateBrush* ImageBrush = Node->IsInfiniteDuration() 
		? FBangoEditorStyle::GetImageBrush(BangoEditorBrushes.Icon_PauseHand) 
		: FBangoEditorStyle::GetImageBrush(BangoEditorBrushes.Icon_Hourglass);
	
	return ImageBrush;
}

TOptional<TTransform2<float>> SGraphNode_BangoSleep::RenderTransform_Hourglass() const
{
	if (!GEditor)
	{
		return NullOpt;
	}
	
	if (!GEditor->IsPlaySessionInProgress())
	{
		return NullOpt;
	}
	
	FBangoSleepAction* SleepAction = GetSleepAction();
	
	if (SleepAction)
	{
		const float X = SleepAction->Duration - SleepAction->TimeRemaining; // Time elapsed
		const float Bounce = 1.5f; // 1.0 is no effect, negative reduces stopping on 180 points, positive adds some bounceback
		const float Speed = 2.0f; // Period
		const float Rate = 0.5f; // Amplitude
		const float Val = Bounce * Rate * FMath::RadiansToDegrees(Speed / Bounce * PI * X + FMath::Sin(Speed * PI * X - PI));
		return FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(Val)));
	}

	return NullOpt;
}

TSharedPtr<SGraphPin> SGraphNode_BangoSleep::CreatePinWidget(UEdGraphPin* Pin) const
{
    TSharedPtr<SGraphPin> NewPin = SGraphNode::CreatePinWidget(Pin);

	FName PinName = Pin->GetFName();
	static FName DurationPinName(TEXT("Duration"));
	
	if (PinName == UEdGraphSchema_K2::PN_Completed || PinName == DurationPinName)
	{
   		NewPin->SetShowLabel(false);
	}
	
   	return NewPin;
}

void SGraphNode_BangoSleep::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	TSharedPtr<SVerticalBox> DestinationBox;
	TArray<TSharedRef<SGraphPin>>* DestinationPinArray;
	FMargin PinPadding;
	
	if (PinToAdd->GetPinObj()->GetFName() == UEdGraphSchema_K2::PN_Execute)
	{
		DestinationBox = LeftNodeBox;
		DestinationPinArray = &InputPins;
		PinPadding = Settings->GetInputPinPadding();
	}
	else if (PinToAdd->GetPinObj()->GetFName() == UEdGraphSchema_K2::PN_Completed)
	{
		DestinationBox = RightNodeBox;
		DestinationPinArray = &OutputPins;
		PinPadding = Settings->GetOutputPinPadding();
	}
	else
	{
		DestinationBox = InnerVerticalBox;
		DestinationPinArray = &InputPins;
		PinPadding = Settings->GetInputPinPadding();
	}
	
	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}

	DestinationBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(PinPadding)
		[
			PinToAdd
		];
	DestinationPinArray->Add(PinToAdd);
}

void SGraphNode_BangoSleep::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	FKismetNodeInfoContext* K2Context = (FKismetNodeInfoContext*)Context;

	// Display any pending latent actions
	if (UObject* ActiveObject = K2Context->ActiveObjectBeingDebugged)
	{
		TArray<FKismetNodeInfoContext::FObjectUUIDPair>* Pairs = K2Context->NodesWithActiveLatentActions.Find(GraphNode);
		if (Pairs != NULL)
		{
			for (int32 Index = 0; Index < Pairs->Num(); ++Index)
			{
				FKismetNodeInfoContext::FObjectUUIDPair Action = (*Pairs)[Index];

				if (Action.Object == ActiveObject)
				{
					if (UWorld* World = GEngine->GetWorldFromContextObject(Action.Object, EGetWorldErrorMode::ReturnNull))
					{
						FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

						const FString LatentDesc = LatentActionManager.GetDescription(Action.Object, Action.UUID);
						const FString& ActorLabel = Action.GetDisplayName();

						new (Popups) FGraphInformationPopupInfo(NULL, LatentBubbleColor, LatentDesc);
					}
				}
			}
		}
		
		// Display pinned watches
		if (K2Context->WatchedNodeSet.Contains(GraphNode))
		{
			const UBlueprintEditorSettings* BlueprintEditorSettings = GetDefault<UBlueprintEditorSettings>();
			UBlueprint* Blueprint = K2Context->SourceBlueprint;
			const UEdGraphSchema* Schema = GraphNode->GetSchema();

			const FPerBlueprintSettings* FoundSettings = BlueprintEditorSettings->PerBlueprintSettings.Find(Blueprint->GetPathName());
			if (FoundSettings)
			{
				FString PinnedWatchText;
				int32 ValidWatchCount = 0;
				TMap<const UEdGraphPin*, TSharedPtr<FPropertyInstanceInfo>> CachedPinInfo;
				for (const FBlueprintWatchedPin& WatchedPin : FoundSettings->WatchedPins)
				{
					const UEdGraphPin* WatchPin = WatchedPin.Get();
					if (WatchPin && WatchPin->GetOwningNode() == GraphNode)
					{
						if (ValidWatchCount > 0)
						{
							PinnedWatchText += TEXT("\n");
						}
						TSharedPtr<FPropertyInstanceInfo> PinInfo;
						if (CachedPinInfo.Find(WatchPin))
						{
							PinInfo = CachedPinInfo[WatchPin];
						}
						else
						{
							const FKismetDebugUtilities::EWatchTextResult WatchStatus = FKismetDebugUtilities::GetDebugInfo(CachedPinInfo.Add(WatchPin), Blueprint, ActiveObject, WatchPin);

							if (WatchStatus == FKismetDebugUtilities::EWTR_Valid)
							{
								PinInfo = CachedPinInfo[WatchPin];
							}
							else
							{
								FString PinName = UEdGraphSchema_K2::TypeToText(WatchPin->PinType).ToString();
								PinName += TEXT(" ");
								PinName += Schema->GetPinDisplayName(WatchPin).ToString();

								switch (WatchStatus)
								{
								case FKismetDebugUtilities::EWTR_Valid:
									break;

								case FKismetDebugUtilities::EWTR_NotInScope:
									PinnedWatchText += FText::Format(LOCTEXT("WatchingWhenNotInScopeFmt", "Watching {0}\n\t(not in scope)"), FText::FromString(PinName)).ToString();
									break;

								case FKismetDebugUtilities::EWTR_NoProperty:
									PinnedWatchText += FText::Format(LOCTEXT("WatchingUnknownPropertyFmt", "Watching {0}\n\t(no debug data)"), FText::FromString(PinName)).ToString();
									break;

								default:
								case FKismetDebugUtilities::EWTR_NoDebugObject:
									PinnedWatchText += FText::Format(LOCTEXT("WatchingNoDebugObjectFmt", "Watching {0}"), FText::FromString(PinName)).ToString();
									break;
								}
							}
						}

						if (PinInfo.IsValid())
						{
							FString WatchName;
							FString WatchText;
							if (WatchedPin.GetPathToProperty().IsEmpty())
							{
								WatchName = UEdGraphSchema_K2::TypeToText(WatchPin->PinType).ToString();
								WatchName += TEXT(" ");
								WatchName += Schema->GetPinDisplayName(WatchPin).ToString();

								WatchText = PinInfo->GetWatchText();
							}
							else
							{
								TSharedPtr<FPropertyInstanceInfo> PropWatch = PinInfo->ResolvePathToProperty(WatchedPin.GetPathToProperty());
								if (PropWatch.IsValid())
								{
									WatchName = UEdGraphSchema_K2::TypeToText(PropWatch->Property.Get()).ToString();
									WatchName += TEXT(" ");

									WatchText = PropWatch->GetWatchText();
								}
								else
								{
									WatchText = LOCTEXT("NoDebugData", "(no debug data)").ToString();
								}
								
								WatchName += Schema->GetPinDisplayName(WatchPin).ToString();

								for (const FName& PathName : WatchedPin.GetPathToProperty())
								{
									if (!PathName.ToString().StartsWith("["))
									{
										WatchName += TEXT("/");
									}

									WatchName += PathName.ToString();
								}
							}

							PinnedWatchText += FText::Format(LOCTEXT("WatchingAndValidFmt", "Watching {0}\n\t{1}"), FText::FromString(WatchName), FText::FromString(WatchText)).ToString();
						}

						ValidWatchCount++;
					}
				}

				if (ValidWatchCount)
				{
					new (Popups) FGraphInformationPopupInfo(nullptr, PinnedWatchColor, PinnedWatchText);
				}
			}
		}
	}
}

EVisibility SGraphNode_BangoSleep::Visibility_ProgressBar() const
{
	if (GEditor && GEditor->IsPlaySessionInProgress())
	{
		return EVisibility::Visible;
	}

	return EVisibility::Hidden;
}

FSlateColor SGraphNode_BangoSleep::ColorAndOpacity_Hourglass() const
{
	if (!GEditor)
	{
		return FLinearColor::Black;
	}
	
	FLinearColor EditorColor = 0.8f * FLinearColor::White;
	
	if (GEditor && !GEditor->IsPlaySessionInProgress())
	{
		return EditorColor;
	}
	
	FBangoSleepAction* SleepAction = GetSleepAction();
	
	if (SleepAction)
	{
		if (SleepAction->bPaused)
		{
			return FColor::Orange;
		}
		
		return FLinearColor::White;
	}

	UWorld* World = GEditor->GetCurrentPlayWorld(GEditor->PlayWorld);
	
	if (!World)
	{
		return EditorColor;
	}

	const FLinearColor IdleColor = 0.2f * FLinearColor::White;
	FLinearColor FinalColor = IdleColor;
	
	if (AbortTime > 0.0f)
	{
		float AbortTimeElapsed = World->GetTimeSeconds() - AbortTime;
		const float FadeDuration = 5.0f;

		if (AbortTimeElapsed <= FadeDuration)
		{
			float Lerp = FMath::Clamp(FMath::Pow(FMath::Lerp(0.0f, 1.0f, AbortTimeElapsed / FadeDuration), 4.0f), 0.0f, 1.0f);
			FinalColor = FMath::Lerp(FLinearColor::Red, IdleColor, Lerp);
		}
	}
	else if (SkipTime > 0.0f)
	{
		float SkipTimeElapsed = World->GetTimeSeconds() - SkipTime;
		const float FadeDuration = 5.0f;

		if (SkipTimeElapsed <= FadeDuration)
		{
			float Lerp = FMath::Clamp(FMath::Pow(FMath::Lerp(0.0f, 1.0f, SkipTimeElapsed / FadeDuration), 4.0f), 0.0f, 1.0f);
			FinalColor = FMath::Lerp(FLinearColor::Green, IdleColor, Lerp);
		}
	}

	return FinalColor;
}

FBangoSleepAction* SGraphNode_BangoSleep::GetSleepAction() const
{
	FBangoKismetNodeInfoContext K2Context(GetNodeObj()->GetGraph());
	
	if (UObject* ActiveObject = K2Context.ActiveObjectBeingDebugged)
	{
		TArray<FBangoKismetNodeInfoContext::FObjectUUIDPair>* Pairs = K2Context.NodesWithActiveLatentActions.Find(GraphNode);
		
		if (Pairs != NULL)
		{
			for (int32 Index = 0; Index < Pairs->Num(); ++Index)
			{
				FBangoKismetNodeInfoContext::FObjectUUIDPair Action = (*Pairs)[Index];

				if (Action.Object == ActiveObject)
				{
					if (UWorld* World = GEngine->GetWorldFromContextObject(Action.Object, EGetWorldErrorMode::ReturnNull))
					{
						FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
						
						return LatentActionManager.FindExistingAction<FBangoSleepAction>(Action.Object, Action.UUID);
					}
				}
			}
		}
	}

	return nullptr;
}

UK2Node_BangoSleep* SGraphNode_BangoSleep::GetSleepNode() const
{
	return Cast<UK2Node_BangoSleep>(GetNodeObj());
}

void SGraphNode_BangoSleep::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeK2Base::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	FBangoSleepAction* SleepAction = GetSleepAction();
	
	if (SleepAction)
	{
		if (CurrentSleepAction != SleepAction)
		{
			AbortTime = -1.0f;
			SkipTime = -1.0f;

			CurrentSleepAction = SleepAction;
			CurrentSleepAction->OnCancel.AddRaw(this, &SGraphNode_BangoSleep::OnAborted);
			CurrentSleepAction->OnSkip.AddRaw(this, &SGraphNode_BangoSleep::OnSkip);
		}
	}
}

void SGraphNode_BangoSleep::OnAborted()
{
	if (GEditor)
	{
		UWorld* World = GEditor->GetCurrentPlayWorld();

		if (World)
		{
			AbortTime = GEditor->GetCurrentPlayWorld()->GetTimeSeconds();
		}
	}
}

void SGraphNode_BangoSleep::OnSkip()
{
	if (GEditor)
	{
		UWorld* World = GEditor->GetCurrentPlayWorld();

		if (World)
		{
			SkipTime = GEditor->GetCurrentPlayWorld()->GetTimeSeconds();
		}
	}
}

#undef LOCTEXT_NAMESPACE
