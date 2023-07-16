#include "Puny/PunyEventComponent.h"

#include "Bango/Utility/Log.h"
#include "Puny/PunyTrigger.h"
#include "Puny/PunyAction.h"
#include "Puny/PunyEvent.h"
#include "Puny/PunyEventSignal.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

UPunyEventComponent::UPunyEventComponent()
{
}

void UPunyEventComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(Event))
	{
		UE_LOG(Bango, Error, TEXT("UPunyEventComponent of <%s> has no event handler set!"), *GetOwner()->GetName());
		return;
	}

	Event->Init();
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		Trigger->RegisterEvent(Event);
		Trigger->SetEnabled(true);
	}

	for (UPunyAction* Action : Actions)
	{
		Event->RegisterAction(Action);
	}
}

void UPunyEventComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!IsValid(Event))
	{
		UE_LOG(Bango, Error, TEXT("UPunyEventComponent of <%s> has no event handler set!"), *GetOwner()->GetName());
		return;
	}
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		Trigger->UnregisterEvent(Event);
		Trigger->SetEnabled(false);
	}

	for (UPunyAction* Action : Actions)
	{
		Event->UnregisterAction(Action);
	}
	
	Super::EndPlay(EndPlayReason);
}

FText UPunyEventComponent::GetDisplayName()
{
	if (bUseDisplayName)
	{
		if (DisplayName.IsEmpty())
		{
			return FText::FromString("UNNAMED");
		}
		
		return DisplayName;
	}
	
	return FText::FromString(GetOwner()->GetActorNameOrLabel());
}
