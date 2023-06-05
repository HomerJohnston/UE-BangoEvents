// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoTrigger.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Log.h"


bool UBangoTrigger::GetCanActivateEvent()
{
	return bCanActivateEvent;
}

bool UBangoTrigger::GetCanDeactivateEvent()
{
	return bCanDeactivateEvent;
}

ABangoEvent* UBangoTrigger::GetEvent()
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoTrigger::SetEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		OnTriggerActivation.BindDynamic(GetEvent(), &ABangoEvent::Activate);
		OnTriggerDeactivation.BindDynamic(GetEvent(), &ABangoEvent::Deactivate);
		Enable();
	}
	else
	{
		OnTriggerActivation.Clear();
		OnTriggerDeactivation.Clear();
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
	if (!bCanActivateEvent)
	{
		return;
	}
	
	if (GetEvent()->GetIsFrozen())
	{
		return;
	}

	OnTriggerActivation.Execute(NewInstigator);
}

void UBangoTrigger::DeactivateEvent(UObject* OldInstigator)
{
	if (!bCanDeactivateEvent)
	{
		return;
	}
	
	OnTriggerDeactivation.Execute(OldInstigator);	
}

FText UBangoTrigger::GetDisplayName()
{
	return GetClass()->GetDisplayNameText();
}
