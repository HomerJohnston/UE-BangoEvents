#include "Puny/PunyEventActor.h"

#include "Puny/PunyEventComponent.h"

APunyEventActor::APunyEventActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	EventComponent = CreateDefaultSubobject<UPunyEventComponent>("EventComponent");
}