﻿#include "Puny/Default/_EventBaseActor.h"

#include "Puny/Core/EventComponent.h"

APunyEventBaseActor::APunyEventBaseActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	EventComponent = CreateDefaultSubobject<UPunyEventComponent>("EventComponent");
	EventComponent->SetupAttachment(RootComponent);
}