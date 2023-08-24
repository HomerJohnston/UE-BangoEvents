#pragma once

#include "IDetailCustomization.h"

enum class EBangoTriggerSignalType : uint8;
class UBangoEventComponent;

class FBangoEventComponentDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void DrawRuntimeDetails(IDetailLayoutBuilder& DetailBuilder);

	void DrawEditorDetails(IDetailLayoutBuilder& DetailBuilder);
	
	FReply ClickActivateEvent();
	
	FReply ClickDeactivateEvent();

	FReply HandleClick(EBangoTriggerSignalType SignalType);
	
	TWeakObjectPtr<UBangoEventComponent> EventComponent;

	TSharedPtr<IPropertyHandle> TestInstigatorProperty;
};
