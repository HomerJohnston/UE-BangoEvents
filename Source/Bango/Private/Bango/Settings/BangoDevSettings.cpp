// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Settings/BangoDevSettings.h"

bool UBangoDevSettings::GetShowEventsInGame() const
{
	return bShowEventsInGame;
}

float UBangoDevSettings::GetFarDisplayDistance() const
{
	return FarDisplayDistance;
}

float UBangoDevSettings::GetNearDisplayDistance() const
{
	return NearDisplayDistance;
}

float UBangoDevSettings::GetEventDisplaySize() const
{
	return EventDisplaySize;
}
