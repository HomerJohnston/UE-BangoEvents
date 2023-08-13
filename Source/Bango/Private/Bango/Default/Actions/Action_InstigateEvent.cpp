#include "Bango/Default/Actions/Action_InstigateEvent.h"

#include "Bango/Core/Event.h"
#include "Bango/Core/ActionSignal.h"
#include "Bango/Utility/Log.h"
#include "Bango/Core/EventComponent.h"

UBangoAction_InstigateEvent::UBangoAction_InstigateEvent()
{
	OnStart = EBangoEventSignalType::None;
	OnStop = EBangoEventSignalType::None;
}

void UBangoAction_InstigateEvent::HandleSignal_Implementation(UBangoEvent* Event, FBangoActionSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoActionSignalType::StartAction:
		{
			Handle(OnStart);
			break;
		}
		case EBangoActionSignalType::StopAction:
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

void UBangoAction_InstigateEvent::Handle(EBangoEventSignalType Signal)
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

void UBangoAction_InstigateEvent::HandleComponent(EBangoEventSignalType Signal)
{
	UBangoEventComponent* EventComponent = Cast<UBangoEventComponent>(TargetComponent.GetComponent(GetActor()));

	if (!IsValid(EventComponent))
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_InstigateEvent::HandleComponent - invalid Target Component!"));
		return;
	}

	UBangoEvent* Event = EventComponent->GetEvent();
	
	if (!IsValid(Event))
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_InstigateEvent::HandleComponent - invalid Target Component Event!"));
		return;
	}

	FBangoEventSignal Trigger(Signal, this);

	Event->RespondToTriggerSignal(nullptr, Trigger);
}

void UBangoAction_InstigateEvent::HandleActor(EBangoEventSignalType Signal)
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_InstigateEvent::HandleActor - invalid Target Actor!"));
		return;
	}
	
	for (UActorComponent* Component : TargetActor->GetComponents())
	{
		if (!IsValid(Component))
		{
			return;
		}

		UBangoEventComponent* EventComponent = Cast<UBangoEventComponent>(Component);

		if (!IsValid(EventComponent))
		{
			continue;
		}

		UBangoEvent* Event = EventComponent->GetEvent();
	
		if (!IsValid(Event))
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_InstigateEvent::HandleComponent - invalid Target Component Event!"));
			return;
		}

		FBangoEventSignal Trigger(Signal, this);

		Event->RespondToTriggerSignal(nullptr, Trigger);
	}
}

bool UBangoAction_InstigateEvent::HasValidSetup()
{
	return Super::HasValidSetup();
}

void UBangoAction_InstigateEvent::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	Super::DebugDraw_Implementation(Canvas, Cont);
}
