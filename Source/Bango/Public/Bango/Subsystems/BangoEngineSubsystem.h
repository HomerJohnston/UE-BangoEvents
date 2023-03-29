#pragma once

#include "Subsystems/EngineSubsystem.h"

#include "BangoEngineSubsystem.generated.h"


class ABangoEvent;
UCLASS()
class BANGO_API UBangoEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	void RegisterBangoEvent(ABangoEvent* Event);
	
	void UnregisterBangoEvent(ABangoEvent* BangoEvent);

	UPROPERTY()
	TSet<ABangoEvent*> RegisteredEvents;
	
	TMulticastDelegate<void(ABangoEvent*)> OnEventAltered;
};
