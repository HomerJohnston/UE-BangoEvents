#include "BangoScripts/Editor/Widgets/SBangoTriggerEditorSection.h"

#include "Components/VerticalBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"

void SBangoTriggerEditorSection::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8, 8, 0, 0)
		[
			SNew(STextBlock)
			.Text(InArgs._Title)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 8, 0, 0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				InArgs._Content.Widget
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SSpacer)
			]
		]
	];
}
