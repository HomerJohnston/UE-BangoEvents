#include "Bango/DefaultImpl/Actions/BangoAction_ActivateDeactivateEvent.h"
#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"

UBangoAction_ActivateDeactivateEvent::UBangoAction_ActivateDeactivateEvent()
{
	OnStartAction = EBangoActivateDeactivateEventAction::DoNothing;
	
	OnStopAction = EBangoActivateDeactivateEventAction::DoNothing;
}

void UBangoAction_ActivateDeactivateEvent::OnStart_Implementation()
{
	Execute(OnStartAction);
}

void UBangoAction_ActivateDeactivateEvent::OnStop_Implementation()
{
	Execute(OnStopAction);
}

void UBangoAction_ActivateDeactivateEvent::Execute(EBangoActivateDeactivateEventAction Action)
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
	
	switch (Action)
	{
		case EBangoActivateDeactivateEventAction::ActivateEvent:
		{
			//TargetEvent->Activate(GetEvent());
			break;
		}
		case EBangoTriggerInstigatorAction::RemoveInstigator:
		{
			//TargetEvent->Deactivate(GetEvent());
			break;
		}
		default:
		{
			break;
		}
	}
}
