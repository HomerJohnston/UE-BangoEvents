#include "Bango/BangoTrigger_OLD.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoEventComponent.h"
#include "Bango/Core/BangoTriggerSignal.h"
#include "Bango/BangoInstigatorFilter.h"

void UBangoTrigger_OLD::SetEnabled(bool bEnabled)
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

void UBangoTrigger_OLD::RegisterEvent(UBangoEvent* Event)
{
	TriggerSignal.BindDynamic(Event, &UBangoEvent::RespondToTriggerSignal);
}

void UBangoTrigger_OLD::UnregisterEvent(UBangoEvent* Event)
{
	TriggerSignal.Clear();
}

void UBangoTrigger_OLD::Enable_Implementation()
{
	checkNoEntry();
}

void UBangoTrigger_OLD::Disable_Implementation()
{
	checkNoEntry();
}

void UBangoTrigger_OLD::SendSignal(FBangoTriggerSignal Signal)
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

UBangoEventComponent* UBangoTrigger_OLD::GetEventComponent()
{
	return Cast<UBangoEventComponent>(GetOuter());
}

AActor* UBangoTrigger_OLD::GetActor()
{
	return GetEventComponent()->GetOwner();
}

#if WITH_EDITOR
FText UBangoTrigger_OLD::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}
#endif

#if WITH_EDITOR
void UBangoTrigger_OLD::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	
}
#endif

#if WITH_EDITOR
bool UBangoTrigger_OLD::HasValidSetup()
{
	return true;
}
#endif

#if WITH_EDITOR
void UBangoTrigger_OLD::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	// Child classes may implement custom draw logic here
}
#endif
