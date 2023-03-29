#include "Bango/Subsystems/BangoEngineSubsystem.h"

void UBangoEngineSubsystem::RegisterBangoEvent(ABangoEvent* Event)
{
	if (!RegisteredEvents.Contains(Event))
	{
		RegisteredEvents.Add(Event);
		RegisteredEvents.Compact();
	}

	OnEventAltered.Broadcast(Event);
}

void UBangoEngineSubsystem::UnregisterBangoEvent(ABangoEvent* BangoEvent)
{
	RegisteredEvents.Remove(BangoEvent);
}
