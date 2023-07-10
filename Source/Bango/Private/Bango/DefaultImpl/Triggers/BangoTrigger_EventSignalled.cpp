#include "Bango/DefaultImpl/Triggers/BangoTrigger_EventSignalled.h"

#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Core/BangoSignal.h"

UBangoTrigger_EventSignalled::UBangoTrigger_EventSignalled()
{
	bUseWatchedEventInstigator = true;
}

void UBangoTrigger_EventSignalled::Enable_Implementation()
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

	WatchedEvent->OnEventTriggered.AddDynamic(this, &ThisClass::OnTargetEventSignalled);
}

void UBangoTrigger_EventSignalled::Disable_Implementation()
{
	WatchedEvent->OnEventTriggered.RemoveDynamic(this, &ThisClass::OnTargetEventSignalled);
}

void UBangoTrigger_EventSignalled::OnTargetEventSignalled(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator)
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
