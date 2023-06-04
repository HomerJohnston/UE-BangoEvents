// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

void UBangoAction::Start(ABangoEvent* EventActor, UObject* NewInstigator)
{
	Event = EventActor;
	Instigator = NewInstigator;

	check(Event);
	check(Instigator);
	
	OnStart();
}

void UBangoAction::Stop()
{
	OnStop();
}

void UBangoAction::OnStart_Implementation()
{
}

void UBangoAction::OnStop_Implementation()
{
}

FText UBangoAction::GetDisplayName_Implementation()
{
	return DisplayName;
}
