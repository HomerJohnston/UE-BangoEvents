// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

void UBangoAction::StartInternal(ABangoEvent* EventActor, UObject* NewInstigator)
{
	Event = EventActor;
	Instigator = NewInstigator;

	check(Event);
	check(Instigator);
	
	Start();
}

void UBangoAction::StopInternal()
{
	Stop();
}

void UBangoAction::Start_Implementation()
{
}

void UBangoAction::Stop_Implementation()
{
}

FText UBangoAction::GetDisplayName_Implementation()
{
	return DisplayName;
}
