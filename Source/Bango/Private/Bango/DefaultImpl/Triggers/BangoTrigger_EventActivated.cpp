#include "Bango/DefaultImpl/Triggers/BangoTrigger_EventActivated.h"

#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Core/BangoSignal.h"

UBangoTrigger_EventActivated::UBangoTrigger_EventActivated()
{
	bUseWatchedEventInstigator = true;
}

void UBangoTrigger_EventActivated::Enable_Implementation()
{
	if (WatchedEvent.IsPending())
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is not loaded!"));
		return;
	}
	
	if (!WatchedEvent.IsValid())
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is invalid!"));
		return;
	}

	WatchedEvent->OnBangoEventTriggered.AddDynamic(this, &ThisClass::OnTargetEventSignalled);
}

void UBangoTrigger_EventActivated::Disable_Implementation()
{
	WatchedEvent->OnBangoEventTriggered.RemoveDynamic(this, &ThisClass::OnTargetEventSignalled);
}

void UBangoTrigger_EventActivated::OnTargetEventSignalled(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator)
{
	if (WatchedEvent.IsPending())
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is not loaded!"));
		return;
	}
	
	if (!WatchedEvent.IsValid())
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is invalid!"));
		return;
	}

	EBangoSignal* SignalToSend = ActionSignalMap.Find(Signal);

	if (SignalToSend)
	{
		SendTriggerSignal(*SignalToSend, SignalInstigator);
	}
}
