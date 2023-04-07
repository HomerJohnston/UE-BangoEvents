// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/TriggerCondition.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Log.h"

void UBangoTriggerCondition::Enable_Implementation()
{
	
}


void UBangoTriggerCondition::Disable_Implementation()
{
	
}

ABangoEvent* UBangoTriggerCondition::GetEvent()
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoTriggerCondition::SetEnabled(bool bEnabled)
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

void UBangoTriggerCondition::Trigger(UObject* NewInstigator)
{
	if (GetEvent()->GetIsFrozen())
	{
		UE_LOG(Bango, Warning, TEXT("Trigger <%s> tried to activate event <%s> but event is frozen (did you forget to implement Disable function in trigger?"), *GetName(), *GetEvent()->GetName())
		return;
	}

	OnTrigger.Execute(NewInstigator);
}
