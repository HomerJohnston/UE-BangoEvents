#pragma once
#include "Widgets/SUserWidget.h"

class SBangoTriggerEditorSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBangoTriggerEditorSection)
		: _Title(INVTEXT("Untitled Section"))
		{}

		SLATE_ARGUMENT(FText, Title)

		SLATE_DEFAULT_SLOT(FArguments, Content)
		
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
};
