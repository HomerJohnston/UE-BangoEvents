#include "Bango/DefaultImpl/Triggers/BangoTrigger_EventActivated.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/DefaultImpl/BangoDefaultImplEnums.h"

UBangoTrigger_EventActivated::UBangoTrigger_EventActivated()
{
	OnEventActivatedAction = EBangoActivateDeactivateEventAction::DoNothing;
	OnEventDeactivatedAction = EBangoActivateDeactivateEventAction::DoNothing;
}

void UBangoTrigger_EventActivated::Enable_Implementation()
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

	TargetEvent->OnBangoEventActivated.AddDynamic(this, &ThisClass::OnTargetEventActivated);
	TargetEvent->OnBangoEventDeactivated.AddDynamic(this, &ThisClass::OnTargetEventDeactivated);
}

void UBangoTrigger_EventActivated::Disable_Implementation()
{
	TargetEvent->OnBangoEventActivated.RemoveAll(this);
	TargetEvent->OnBangoEventDeactivated.RemoveAll(this);
}

void UBangoTrigger_EventActivated::OnTargetEventActivated(ABangoEvent* Event, UObject* Instigator)
{
	Execute(OnEventActivatedAction);
}

void UBangoTrigger_EventActivated::OnTargetEventDeactivated(ABangoEvent* Event, UObject* Instigator)
{
	Execute(OnEventDeactivatedAction);
}

void UBangoTrigger_EventActivated::Execute(EBangoActivateDeactivateEventAction Action)
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
			GetEvent()->Activate(this);
			break;
		}
		case EBangoActivateDeactivateEventAction::DeactivateEvent:
		{
			GetEvent()->Deactivate(this);
			break;
		}
		case EBangoActivateDeactivateEventAction::DoNothing:
		{
			break;
		}
	}
}
