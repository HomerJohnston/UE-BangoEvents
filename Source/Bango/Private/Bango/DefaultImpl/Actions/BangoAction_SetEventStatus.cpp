#include "Bango/DefaultImpl/Actions/BangoAction_SetEventStatus.h"

#include "Bango/Core/BangoEvent.h"

void UBangoAction_SetEventStatus::Execute(EBangoAction_FreezeThawEvent_Type Type)
{
	if (!IsValid(TargetEvent))
	{
		return;
	}
	
	switch (Type)
	{
		case EBangoAction_FreezeThawEvent_Type::Active:
			TargetEvent->SetFrozen(false);
			break;
		case EBangoAction_FreezeThawEvent_Type::Frozen:
			TargetEvent->SetFrozen(true);
			break;
	}
}

void UBangoAction_SetEventStatus::OnStart_Implementation()
{
	Execute(StatusToSetOnStart);
}

void UBangoAction_SetEventStatus::OnStop_Implementation()
{
	Execute(StatusToSetOnStop);
}
