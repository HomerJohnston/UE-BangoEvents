#pragma once

#include "Widgets/SUserWidget.h"

class UBangoTargetCollector;
enum class EBangoTriggerType : uint8;
class UBangoTargetGroup;
class ABangoTrigger;

class FBangoTriggerViewportPopup
{
public:
	
	static void Show(ABangoTrigger* Trigger);

	static TSharedRef<SWidget> Get();
	
protected:
	
	static TSharedRef<SWidget> MakeTriggerIDSection();
	
	static TSharedRef<SWidget> MakeTriggerTypeSection();
	
	static TSharedRef<SWidget> MakeTriggerRegionSection();
	
	static TSharedRef<SWidget> MakeTargetsSelectionSection();
	
	static TSharedRef<SWidget> MakeTriggerTypeWidget(EBangoTriggerType TriggerType);
	
	static TSharedRef<SWidget> MakeTargetCollectorWidget(TSubclassOf<UBangoTargetCollector> CollectorClass);
	
	static TSharedRef<SWidget> MakeTypeGroupWidget(TSubclassOf<UBangoTargetGroup> TypeGroupClass);
	
	static TSharedRef<SWidget> Make__Widget(FText Text);
};
