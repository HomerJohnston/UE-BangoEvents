#include "Bango/DefaultImpl/Actions/BangoAction_ControlEvent.h"

#include "Bango/Core/BangoEvent.h"

void UBangoAction_ControlEvent::Execute(EBangoAction_FreezeThawEvent_Type Type)
{
	if (!IsValid(TargetEvent))
	{
		return;
	}
	
	switch (Type)
	{
		case EBangoAction_FreezeThawEvent_Type::Thaw:
			TargetEvent->SetFrozen(false);
			break;
		case EBangoAction_FreezeThawEvent_Type::Freeze:
			TargetEvent->SetFrozen(true);
			break;
	}
}

void UBangoAction_ControlEvent::OnStart_Implementation()
{
	Execute(OnStartAction);
}

void UBangoAction_ControlEvent::OnStop_Implementation()
{
	Execute(OnStopAction);
}
