#include "Puny/Default/Actions/Action_InstigateEvent.h"

#include "Puny/Core/Event.h"
#include "Puny/Core/TriggerSignal.h"
#include "Bango/Utility/Log.h"
#include "Puny/Core/EventComponent.h"

UPunyAction_InstigateEvent::UPunyAction_InstigateEvent()
{
	OnStart = EPunyTriggerSignalType::None;
	OnStop = EPunyTriggerSignalType::None;
}

void UPunyAction_InstigateEvent::HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EPunyEventSignalType::StartAction:
		{
			Handle(OnStart);
			break;
		}
		case EPunyEventSignalType::StopAction:
		{
			Handle(OnStop);
			break;
		}
		default:
		{
			break;
		}
	}
}

void UPunyAction_InstigateEvent::Handle(EPunyTriggerSignalType Signal)
{
	if (bUseTargetComponent)
	{
		HandleComponent(Signal);
	}
	else if (bUseTargetActor)
	{
		HandleActor(Signal);
	}
}

void UPunyAction_InstigateEvent::HandleComponent(EPunyTriggerSignalType Signal)
{
	UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));

	if (!IsValid(EventComponent))
	{
		UE_LOG(Bango, Warning, TEXT("UPunyAction_InstigateEvent::HandleComponent - invalid Target Component!"));
		return;
	}

	UPunyEvent* Event = EventComponent->GetEvent();
	
	if (!IsValid(Event))
	{
		UE_LOG(Bango, Warning, TEXT("UPunyAction_InstigateEvent::HandleComponent - invalid Target Component Event!"));
		return;
	}

	FPunyTriggerSignal Trigger(Signal, this);

	Event->RespondToTriggerSignal(nullptr, Trigger);
}

void UPunyAction_InstigateEvent::HandleActor(EPunyTriggerSignalType Signal)
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(Bango, Warning, TEXT("UPunyAction_InstigateEvent::HandleActor - invalid Target Actor!"));
		return;
	}
	
	for (UActorComponent* Component : TargetActor->GetComponents())
	{
		if (!IsValid(Component))
		{
			return;
		}

		UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(Component);

		if (!IsValid(EventComponent))
		{
			continue;
		}

		UPunyEvent* Event = EventComponent->GetEvent();
	
		if (!IsValid(Event))
		{
			UE_LOG(Bango, Warning, TEXT("UPunyAction_InstigateEvent::HandleComponent - invalid Target Component Event!"));
			return;
		}

		FPunyTriggerSignal Trigger(Signal, this);

		Event->RespondToTriggerSignal(nullptr, Trigger);
	}
}

bool UPunyAction_InstigateEvent::HasValidSetup()
{
	return Super::HasValidSetup();
}

void UPunyAction_InstigateEvent::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	Super::DebugDraw_Implementation(Canvas, Cont);
}
