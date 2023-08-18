#include "Bango/Trigger.h"
#include "Bango/Core/Event.h"
#include "Bango/Core/EventComponent.h"
#include "Bango/Core/TriggerSignal.h"
#include "Bango/BangoInstigatorFilter.h"

void UBangoTrigger::SetEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

void UBangoTrigger::RegisterEvent(UBangoEvent* Event)
{
	TriggerSignal.BindDynamic(Event, &UBangoEvent::RespondToTriggerSignal);
}

void UBangoTrigger::UnregisterEvent(UBangoEvent* Event)
{
	TriggerSignal.Clear();
}

void UBangoTrigger::Enable_Implementation()
{
	checkNoEntry();
}

void UBangoTrigger::Disable_Implementation()
{
	checkNoEntry();
}

void UBangoTrigger::SendSignal(FBangoTriggerSignal Signal)
{
	if  (Signal.Type == EBangoTriggerSignalType::None)
	{
		return;
	}
	
	if (IsValid(InstigatorFilter))
	{
		if (!InstigatorFilter->IsValidInstigator(Signal.Instigator))
		{
			return;
		}
	}
	
	TriggerSignal.Execute(this, Signal);
}

UBangoEventComponent* UBangoTrigger::GetEventComponent()
{
	return Cast<UBangoEventComponent>(GetOuter());
}

AActor* UBangoTrigger::GetActor()
{
	return GetEventComponent()->GetOwner();
}

FText UBangoTrigger::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}

void UBangoTrigger::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	
}

bool UBangoTrigger::HasValidSetup()
{
	return true;
}

void UBangoTrigger::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	// Child classes may implement custom draw logic here
}
