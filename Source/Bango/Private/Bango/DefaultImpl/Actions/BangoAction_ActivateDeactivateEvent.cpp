#include "Bango/DefaultImpl/Actions/BangoAction_ActivateDeactivateEvent.h"
#include "Bango/Utility/Log.h"
#include "Bango/Event/BangoEvent.h"

UBangoAction_ActivateDeactivateEvent::UBangoAction_ActivateDeactivateEvent()
{
}

inline void UBangoAction_ActivateDeactivateEvent::ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator)
{
	EBangoSignal* SendSignal = ActionSignalMap.Find(Signal);

	if (SendSignal)
	{
		Execute(*SendSignal);
	}
}


void UBangoAction_ActivateDeactivateEvent::Execute(EBangoSignal Signal)
{
	if (TargetEvent.IsPending())
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is not loaded!"));
		return;
	}
	
	if (!TargetEvent.IsValid())
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is invalid!"));
		return;
	}

	TargetEvent->ProcessTriggerSignal(Signal, GetEvent());
}
