#include "Bango/Default/BangoEventBaseActor.h"

#include "Bango/Core/BangoEventComponent.h"

ABangoEventBaseActor::ABangoEventBaseActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	EventComponent = CreateDefaultSubobject<UBangoEventComponent>("EventComponent");
	EventComponent->SetupAttachment(RootComponent);
}