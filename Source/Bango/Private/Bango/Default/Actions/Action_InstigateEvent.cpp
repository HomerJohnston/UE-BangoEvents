#include "Bango/Default/Actions/Action_InstigateEvent.h"

#include "Bango/Core/Event.h"
#include "Bango/Core/TriggerSignal.h"
#include "Bango/Utility/Log.h"
#include "Bango/Core/EventComponent.h"

UBangoAction_InstigateEvent::UBangoAction_InstigateEvent()
{
	OnStart = EBangoTriggerSignalType::None;
	OnStop = EBangoTriggerSignalType::None;
}

void UBangoAction_InstigateEvent::HandleSignal_Implementation(UBangoEvent* Event, FBangoEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoEventSignalType::StartAction:
		{
			Handle(OnStart);
			break;
		}
		case EBangoEventSignalType::StopAction:
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

void UBangoAction_InstigateEvent::Handle(EBangoTriggerSignalType Signal)
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

void UBangoAction_InstigateEvent::HandleComponent(EBangoTriggerSignalType Signal)
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

	FBangoTriggerSignal Trigger(Signal, this);

	Event->RespondToTriggerSignal(nullptr, Trigger);
}

void UBangoAction_InstigateEvent::HandleActor(EBangoTriggerSignalType Signal)
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

		FBangoTriggerSignal Trigger(Signal, this);

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
