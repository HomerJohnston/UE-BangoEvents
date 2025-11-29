#include "BangoEditor/Windows/BangoTriggerViewportPopup.h"

#include "Bango/Triggers/BangoTrigger.h"
#include "Bango/Triggers/TargetGroups/Base/BangoTargetGroup.h"
#include "Bango/Triggers/TargetCollectors/Base/BangoTargetCollector.h"
#include "BangoEditor/Widgets/SBangoTriggerEditorSection.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SSegmentedControl.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

void FBangoTriggerViewportPopup::Show(ABangoTrigger* Trigger)
{
	FVector2D ScreenPos(800, 400);
	
	FSlateApplication::Get().PushMenu(
		FSlateApplication::Get().GetUserFocusedWidget(0).ToSharedRef(),
		FWidgetPath(), 
		Get(),
		FDeprecateSlateVector2D(ScreenPos.X, ScreenPos.Y),
		FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup),
		true);
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::Get()
{	
	return SNew(SBorder)
	.Padding(12)
	[
		SNew(SBox)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				MakeTriggerIDSection()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 8, 0, 0)
			[
				MakeTriggerTypeSection()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 8, 0, 0)
			[
				MakeTriggerRegionSection()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 8, 0, 0)
			[
				MakeTargetsSelectionSection()
			]
		]
	];
}


TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTriggerIDSection()
{
	return SNew(SBangoTriggerEditorSection)
	.Title(LOCTEXT("BangoTriggerEditor_ID_Title", "Trigger ID"))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.MinDesiredWidth(200)
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("BangoTriggerEditor_ID_Name_Hint", "Bango ID (Optional)"))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 4, 0, 0)
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("BangoTriggerEditor_ID_Desc_Hint", "Description (Editor-Only)"))
		]
	];
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTriggerTypeSection()
{
	TSharedRef<SSegmentedControl<EBangoTriggerType>> TypeSelector = SNew(SSegmentedControl<EBangoTriggerType>);
	
	for (EBangoTriggerType Type : TEnumRange<EBangoTriggerType>())
	{
		TypeSelector->AddSlot(Type)
		[
			MakeTriggerTypeWidget(Type)
		];
	}
	
	return SNew(SBangoTriggerEditorSection)
	.Title(LOCTEXT("BangoTriggerEditor_Type_Title", "Type"))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			TypeSelector
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(20, 0, 8, 0)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("BangoTriggerEditor_Type_ActLimitLabel", "Limit:"))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(80)
				.HAlign(HAlign_Left)
				[
					SNew(SNumericEntryBox<uint32>)
				]
			]
		]
	];
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTriggerRegionSection()
{
	TSharedRef<SSegmentedControl<TSubclassOf<UBangoTargetCollector>>> CollectorTypeSelector = SNew(SSegmentedControl<TSubclassOf<UBangoTargetCollector>>)
		.SupportsEmptySelection(false)
		.SupportsMultiSelection(false)
		;

	TArray<TSubclassOf<UBangoTargetCollector>> CollectorTypes = UBangoTargetCollector::GetAllTypes<UBangoTargetCollector>();
	
	for (auto TargetGroup : CollectorTypes)
	{
		CollectorTypeSelector->AddSlot(TargetGroup, false)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			MakeTargetCollectorWidget(TargetGroup)
		];
	}
	
	CollectorTypeSelector->RebuildChildren();
	
	
	return SNew(SBangoTriggerEditorSection)
	.Title(LOCTEXT("BangoTriggerEditor_Region_Title", "Area"))
	[
		CollectorTypeSelector
	];
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTargetsSelectionSection()
{
	TSharedRef<SSegmentedControl<TSubclassOf<UBangoTargetGroup>>> TypeGroupsSelector = SNew(SSegmentedControl<TSubclassOf<UBangoTargetGroup>>);

	TArray<TSubclassOf<UBangoTargetGroup>> TargetGroups = UBangoTargetGroup::GetAllTypes<UBangoTargetGroup>();
	
	for (auto TargetGroup : TargetGroups)
	{
		TypeGroupsSelector->AddSlot(TargetGroup, false)
		[
			MakeTypeGroupWidget(TargetGroup)
		];
	}
	
	TypeGroupsSelector->RebuildChildren();
	
	return SNew(SBangoTriggerEditorSection)
	.Title(LOCTEXT("BangoTriggerEditor_Targets_Title", "Targets"))
	[
		TypeGroupsSelector
	];
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTriggerTypeWidget(EBangoTriggerType TriggerType)
{
	FText DisplayName = FText::FromString(StaticEnum<EBangoTriggerType>()->GetNameStringByValue((uint64)TriggerType));
	
	return Make__Widget(DisplayName);
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTargetCollectorWidget(TSubclassOf<UBangoTargetCollector> CollectorClass)
{
	FText DisplayName = CollectorClass.GetDefaultObject()->GetDisplayName();
	
	if (DisplayName.IsEmpty())
	{
		DisplayName = FText::FromString(CollectorClass->GetName());
	}
	
	return Make__Widget(DisplayName);
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::MakeTypeGroupWidget(TSubclassOf<UBangoTargetGroup> TypeGroupClass)
{
	FText DisplayName = TypeGroupClass.GetDefaultObject()->GetDisplayName();
	
	if (DisplayName.IsEmpty())
	{
		DisplayName = FText::FromString(TypeGroupClass->GetName());
	}
	
	return Make__Widget(DisplayName);
}

TSharedRef<SWidget> FBangoTriggerViewportPopup::Make__Widget(FText Text)
{
	return SNew(SBox)
	.HeightOverride(24)
	.MinDesiredWidth(100)
	.MaxDesiredWidth(200)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(Text)
	];
}

#undef LOCTEXT_NAMESPACE
