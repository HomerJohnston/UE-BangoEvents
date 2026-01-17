#include "BangoScripts/Default/Actions/BangoAction_InstigateEvent.h"

#include "BangoScripts/Core/BangoEvent.h"
#include "BangoScripts/Utility/BangoLog.h"
#include "BangoScripts/Core/BangoEventComponent.h"
#include "GameFramework/Actor.h"

UBangoAction_InstigateEvent::UBangoAction_InstigateEvent()
{
#if WITH_EDITORONLY_DATA
	ActionFunctions.Add("SendActivateSignal");
	ActionFunctions.Add("SendDeactivateSignal");
#endif
}

void UBangoAction_InstigateEvent::SendActivateSignal(UBangoEvent* Event, UObject* Instigator)
{
	if (bUseTargetComponent)
	{
		HandleComponent(EBangoTriggerSignalType::ActivateEvent);
	}
	else if (bUseTargetActor)
	{
		HandleActor(EBangoTriggerSignalType::ActivateEvent);
	}
}

void UBangoAction_InstigateEvent::SendDeactivateSignal(UBangoEvent* Event, UObject* Instigator)
{
	if (bUseTargetComponent)
	{
		HandleComponent(EBangoTriggerSignalType::DeactivateEvent);
	}
	else if (bUseTargetActor)
	{
		HandleActor(EBangoTriggerSignalType::DeactivateEvent);
	}
}

void UBangoAction_InstigateEvent::HandleComponent(EBangoTriggerSignalType Signal)
{
	UBangoEventComponent* EventComponent = Cast<UBangoEventComponent>(TargetComponent.GetComponent(GetActor()));

	if (!IsValid(EventComponent))
	{
		UE_LOG(LogBango, Warning, TEXT("UBangoAction_InstigateEvent::HandleComponent - invalid Target Component!"));
		return;
	}

	UBangoEvent* Event = EventComponent->GetEvent();
	
	if (!IsValid(Event))
	{
		UE_LOG(LogBango, Warning, TEXT("UBangoAction_InstigateEvent::HandleComponent - invalid Target Component Event!"));
		return;
	}

	FBangoTriggerSignal Trigger(Signal, this);

	Event->RespondToTriggerSignal(nullptr, Trigger);
}

void UBangoAction_InstigateEvent::HandleActor(EBangoTriggerSignalType Signal)
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogBango, Warning, TEXT("UBangoAction_InstigateEvent::HandleActor - invalid Target Actor!"));
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
			UE_LOG(LogBango, Warning, TEXT("UBangoAction_InstigateEvent::HandleComponent - invalid Target Component Event!"));
			return;
		}

		FBangoTriggerSignal Trigger(Signal, this);

		Event->RespondToTriggerSignal(nullptr, Trigger);
	}
}

#if WITH_EDITOR
bool UBangoAction_InstigateEvent::HasValidSetup()
{
	return Super::HasValidSetup();
}

void UBangoAction_InstigateEvent::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	Super::DebugDraw_Implementation(Canvas, Cont);
}
#endif