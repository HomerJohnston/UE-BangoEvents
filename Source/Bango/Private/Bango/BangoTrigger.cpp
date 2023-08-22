#include "Bango/BangoTrigger.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoEventComponent.h"
#include "Bango/Core/BangoTriggerSignal.h"
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
	if  (Signal.Type == EBangoTriggerSignalType::DoNothing)
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

#if WITH_EDITOR
FText UBangoTrigger::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}
#endif

#if WITH_EDITOR
void UBangoTrigger::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	
}
#endif

#if WITH_EDITOR
bool UBangoTrigger::HasValidSetup()
{
	return true;
}
#endif

#if WITH_EDITOR
void UBangoTrigger::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	// Child classes may implement custom draw logic here
}
#endif
