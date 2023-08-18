#include "Bango/Default/EventBaseActor.h"

#include "Bango/Core/EventComponent.h"

ABangoEventBaseActor::ABangoEventBaseActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	EventComponent = CreateDefaultSubobject<UBangoEventComponent>("EventComponent");
	EventComponent->SetupAttachment(RootComponent);
}