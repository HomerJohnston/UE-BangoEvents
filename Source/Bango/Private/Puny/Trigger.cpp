#include "Puny/Trigger.h"
#include "Puny/Core/Event.h"
#include "Puny/Core/EventComponent.h"
#include "Puny/Core/TriggerSignal.h"
#include "Puny/Core/InstigatorFilter.h"

void UPunyTrigger::SetEnabled(bool bEnabled)
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

void UPunyTrigger::RegisterEvent(UPunyEvent* Event)
{
	TriggerSignal.BindDynamic(Event, &UPunyEvent::RespondToTriggerSignal);
}

void UPunyTrigger::UnregisterEvent(UPunyEvent* Event)
{
	TriggerSignal.Clear();
}

void UPunyTrigger::Enable_Implementation()
{
	checkNoEntry();
}

void UPunyTrigger::Disable_Implementation()
{
	checkNoEntry();
}

void UPunyTrigger::SendSignal(FPunyTriggerSignal Signal)
{
	if  (Signal.Type == EPunyTriggerSignalType::None)
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

UPunyEventComponent* UPunyTrigger::GetEventComponent()
{
	return Cast<UPunyEventComponent>(GetOuter());
}

AActor* UPunyTrigger::GetActor()
{
	return GetEventComponent()->GetOwner();
}

FText UPunyTrigger::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}

void UPunyTrigger::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	
}

bool UPunyTrigger::HasValidSetup()
{
	return true;
}

void UPunyTrigger::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	// Child classes may implement custom draw logic here
}
