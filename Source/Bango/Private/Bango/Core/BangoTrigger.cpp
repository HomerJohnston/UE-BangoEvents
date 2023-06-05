// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoTrigger.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Log.h"


ABangoEvent* UBangoTrigger::GetEvent()
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoTrigger::BindEvent(ABangoEvent* Event)
{
	OnTriggerActivation.BindDynamic(Event, &ABangoEvent::Activate);
	OnTriggerDeactivation.BindDynamic(Event, &ABangoEvent::Deactivate);
}

void UBangoTrigger::SetEnabled(bool bEnabled)
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

void UBangoTrigger::Enable_Implementation()
{
	
}

void UBangoTrigger::Disable_Implementation()
{
	
}

void UBangoTrigger::ActivateEvent(UObject* NewInstigator)
{
	if (GetEvent()->GetIsFrozen())
	{
		UE_LOG(Bango, Warning, TEXT("Trigger <%s> tried to activate event <%s> but event is frozen (did you forget to implement Disable function in trigger?"), *GetName(), *GetEvent()->GetName())
		return;
	}

	OnTriggerActivation.Execute(NewInstigator);
}

void UBangoTrigger::DeactivateEvent(UObject* OldInstigator)
{
	OnTriggerDeactivation.Execute(OldInstigator);	
}
