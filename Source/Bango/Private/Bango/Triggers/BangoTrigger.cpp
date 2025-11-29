#include "Bango/Triggers/BangoTrigger.h"

#include "Components/SphereComponent.h"

ABangoTrigger::ABangoTrigger()
{
	//RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	RootComponent = CreateDefaultSubobject<USphereComponent>("Root");
	// TODO: should I do this?
	//RootComponent->SetMobility(EComponentMobility::Type::Static);
}

void ABangoTrigger::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
}
