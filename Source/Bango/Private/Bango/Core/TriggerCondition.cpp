// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/TriggerCondition.h"

#include "Bango/Core/BangoEvent.h"

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
	OnTrigger.Execute(NewInstigator);
}
