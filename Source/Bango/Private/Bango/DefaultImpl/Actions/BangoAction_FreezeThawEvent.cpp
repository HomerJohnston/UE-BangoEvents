#include "Bango/DefaultImpl/Actions/BangoAction_FreezeThawEvent.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/DefaultImpl/BangoDefaultImplEnums.h"
#include "Bango/Log.h"

UBangoAction_FreezeThawEvent::UBangoAction_FreezeThawEvent()
{
	OnStartAction = EBangoFreezeThawEventAction::DoNothing;
	
	OnStopAction = EBangoFreezeThawEventAction::DoNothing;
}

void UBangoAction_FreezeThawEvent::OnStart_Implementation()
{
	Execute(OnStartAction);
}

void UBangoAction_FreezeThawEvent::OnStop_Implementation()
{
	Execute(OnStopAction);
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