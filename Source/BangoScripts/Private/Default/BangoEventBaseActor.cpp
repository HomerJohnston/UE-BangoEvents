#include "BangoScripts/Default/BangoEventBaseActor.h"

#include "BangoScripts/Core/BangoEventComponent.h"

ABangoEventBaseActor::ABangoEventBaseActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	EventComponent = CreateDefaultSubobject<UBangoEventComponent>("EventComponent");
	EventComponent->SetupAttachment(RootComponent);
}