#include "BangoEditor/Customizations/Details/BangoEventComponentDetailsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoEventComponent.h"
#include "Widgets/Layout/SWrapBox.h"

TSharedRef<IDetailCustomization> FBangoEventComponentDetailsCustomization::MakeInstance()
{
	return MakeShared<FBangoEventComponentDetailsCustomization>();
}

void FBangoEventComponentDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	EventComponent = nullptr;

	DebugInstigatorProperty = DetailBuilder.GetProperty("DebugInstigator");

	DebugInstigatorProperty->MarkHiddenByCustomization();
	
	for (TWeakObjectPtr<UObject>& Object : Objects)
	{
		if (Object->IsA<UBangoEventComponent>())
		{
			EventComponent = Cast<UBangoEventComponent>(Object.Get());
			break;
		}
	}

	if (!EventComponent.IsValid())
	{
		return;
	}

	if (!IsValid(EventComponent->GetWorld()))
	{
		return;
	}

	if (!EventComponent->GetWorld()->IsGameWorld())
	{
		return;
	}

	UBangoEvent* Event = EventComponent->GetEvent();

	if (!IsValid(Event))
	{
		return;
	}
	
	IDetailCategoryBuilder& BangoCategory = DetailBuilder.EditCategory("Bango");

	IDetailGroup& TestGroup = BangoCategory.AddGroup("Testing", INVTEXT("Testing"));

	const FButtonStyle& ButtonStyle = FAppStyle::GetWidgetStyle<FButtonStyle>( "Button" );
	const FTextBlockStyle& NormalText = FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText");
	
	struct FCategoryEntry
	{
		FName CategoryName;
		FName RowTag;
		TSharedPtr<SWrapBox> WrapBox;
		FTextBuilder FunctionSearchText;

		FCategoryEntry(FName InCategoryName)
			: CategoryName(InCategoryName)
		{
			WrapBox = SNew(SWrapBox).UseAllottedSize(true);
		}
	};
	
	TestGroup.AddWidgetRow()
	[
		SNew(SButton)
		.Text(INVTEXT("Activate Event"))
		.TextStyle(&NormalText)
		.ButtonStyle(&ButtonStyle)
		.ContentPadding(FMargin(0.0, 4.0))
		.OnClicked(FOnClicked::CreateSP(this, &FBangoEventComponentDetailsCustomization::ClickActivateEvent))
	];

	TestGroup.AddWidgetRow()
	[
		SNew(SButton)
		.Text(INVTEXT("Deactivate Event"))
		.TextStyle(&NormalText)
		.ButtonStyle(&ButtonStyle)
		.ContentPadding(FMargin(0.0, 4.0))
		.OnClicked(FOnClicked::CreateSP(this, &FBangoEventComponentDetailsCustomization::ClickDeactivateEvent))
	];
	
	TestGroup.AddPropertyRow(DebugInstigatorProperty->AsShared());
}

FReply FBangoEventComponentDetailsCustomization::ClickActivateEvent()
{
	return HandleClick(EBangoTriggerSignalType::ActivateEvent);
}

FReply FBangoEventComponentDetailsCustomization::ClickDeactivateEvent()
{
	return HandleClick(EBangoTriggerSignalType::DeactivateEvent);
}

FReply FBangoEventComponentDetailsCustomization::HandleClick(EBangoTriggerSignalType SignalType)
{
	if (!EventComponent.IsValid())
	{
		return FReply::Unhandled();
	}
	
	UBangoEvent* Event = EventComponent->GetEvent();

	if (!IsValid(Event))
	{
		return FReply::Unhandled();
	}
	
	UObject* Test = nullptr;
	
	DebugInstigatorProperty->GetValue(Test);

	TWeakObjectPtr<UObject> TestInstigator = Test;
	
	Event->RespondToTriggerSignal(nullptr, FBangoTriggerSignal(EBangoTriggerSignalType::ActivateEvent, TestInstigator.Get()));
	
	return FReply::Handled();
}
