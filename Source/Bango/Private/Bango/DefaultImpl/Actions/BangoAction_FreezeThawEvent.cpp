#include "Bango/DefaultImpl/Actions/BangoAction_FreezeThawEvent.h"

#include "Bango/Core/BangoSignal.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Utility/Log.h"

UBangoAction_FreezeThawEvent::UBangoAction_FreezeThawEvent()
{
	BangoUtility::Signals::FillMap(SignalActions, EBangoFreezeThawEventAction::DoNothing);
}

void UBangoAction_FreezeThawEvent::ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator)
{
	EBangoFreezeThawEventAction* Action = SignalActions.Find(Signal);

	if (Action)
	{
		Execute(*Action);
		return;
	}

	UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent - failed to find a valid signal map entry"));
}

void UBangoAction_FreezeThawEvent::Execute(EBangoFreezeThawEventAction Type)
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
	
	switch (Type)
	{
		case EBangoFreezeThawEventAction::UnfreezeEvent:
		{
			TargetEvent->SetFrozen(false);
			break;
		}
		case EBangoFreezeThawEventAction::FreezeEvent:
		{
			TargetEvent->SetFrozen(true);
			break;
		}
		default:
		{
			break;
		}
	}
}
