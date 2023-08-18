#include "Bango/Default/Triggers/BangoTrigger_EventTriggered.h"

#include "Algo/Count.h"
#include "Bango/Utility/BangoLog.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoEventComponent.h"
#include "Bango/Editor/BangoDebugUtility.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/BangoLog.h"

UBangoTrigger_EventTriggered::UBangoTrigger_EventTriggered()
{
	bUseInstigatorFromMasterEvent = true;

	OnTargetEventActivated = EBangoTriggerSignalType::DoNothing;
	OnTargetEventDeactivated = EBangoTriggerSignalType::DoNothing;
}

void UBangoTrigger_EventTriggered::Enable_Implementation()
{	
	for (TSoftObjectPtr<AActor> Actor : MasterEventSources)
	{
		if (Actor.IsPending())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_EventTriggered cannot listen to events on unloaded actor!"));
			return;
		}

		if (!Actor.IsValid())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_EventTriggered cannot listen to events on invalid actor!"));
			return;
		}

		TArray<UBangoEventComponent*> Components;
		Actor->GetComponents<UBangoEventComponent>(Components);

		MasterEvents.Append(Components);
	}
	
	MasterEventsRecentSignals.Reserve(MasterEvents.Num());
	
	for (TSoftObjectPtr<UBangoEventComponent> MasterEvent : MasterEvents)
	{
		if (MasterEvent.IsPending())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_EventTriggered cannot listen to events on unloaded event component!"));
			return;
		}
	
		if (!MasterEvent.IsValid())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_EventTriggered cannot listen to events on invalid event component!"));
			return;
		}

		MasterEvent->OnEventTriggeredDelegate.AddDynamic(this, &ThisClass::OnTargetEventSignalled);

		MasterEventsRecentSignals.Add(EBangoEventSignalType::None);
	}
}

void UBangoTrigger_EventTriggered::Disable_Implementation()
{
	for (TSoftObjectPtr<UBangoEventComponent> MasterEvent : MasterEvents)
	{
		if (!MasterEvent.IsValid())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoTrigger_EventTriggered::Disable_Implementation tried to work on a null event!"));
			continue;
		}
		
		MasterEvent->OnEventTriggeredDelegate.RemoveDynamic(this, &ThisClass::OnTargetEventSignalled);
	}

	MasterEvents.Empty();
}

void UBangoTrigger_EventTriggered::OnTargetEventSignalled(UBangoEventComponent* EventComponent, EBangoEventSignalType EventSignal, UObject* SignalInstigator)
{
	int32 Index = MasterEvents.Find(EventComponent);

	if (Index == INDEX_NONE)
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_ByEventSignal::OnTargetEventSignalled was called with an event that was not set in its list!"));
		return;
	}

	check(MasterEventsRecentSignals.IsValidIndex(Index));
	check(MasterEventsRecentSignals.Num() == MasterEvents.Num());
	
	MasterEventsRecentSignals[Index] = EventSignal;
	
	EBangoTriggerSignalType SignalToPerform = EBangoTriggerSignalType::DoNothing;

	EBangoTriggerSignalType ActivateSignal = OnTargetEventActivated;

	TMap<EBangoEventSignalType, EBangoTriggerSignalType> SignalMap { { EBangoEventSignalType::EventActivated, OnTargetEventActivated }, { EBangoEventSignalType::EventDeactivated, OnTargetEventDeactivated }, { EBangoEventSignalType::None, EBangoTriggerSignalType::DoNothing } };
	
	switch(Requirement)
	{
		case (EBangoEventSignalledRequirement::AnyChanged):
		{
			SignalToPerform = SignalMap[EventSignal];
			break;
		}
		case (EBangoEventSignalledRequirement::AllChanged):
		{
			SignalToPerform = (Algo::Count(MasterEventsRecentSignals, EventSignal) == MasterEventsRecentSignals.Num()) ? SignalMap[EventSignal] : EBangoTriggerSignalType::DoNothing;
			break;
		}
		case (EBangoEventSignalledRequirement::AllActivateAnyDeactivate):
		{
			if (EventSignal == EBangoEventSignalType::EventActivated)
			{
				SignalToPerform = (Algo::Count(MasterEventsRecentSignals, EventSignal) == MasterEventsRecentSignals.Num()) ? SignalMap[EventSignal] : EBangoTriggerSignalType::DoNothing;
				break;
			}
			else if (EventSignal == EBangoEventSignalType::EventDeactivated)
			{
				SignalToPerform = (Algo::Count(MasterEventsRecentSignals, EventSignal) >= 1) ? SignalMap[EventSignal] : EBangoTriggerSignalType::DoNothing;
				break;
			}
			checkNoEntry();
		}
		case (EBangoEventSignalledRequirement::AnyActivateAllDeactivate):
		{
			if (EventSignal == EBangoEventSignalType::EventActivated)
			{
				SignalToPerform = (Algo::Count(MasterEventsRecentSignals, EventSignal) >= 1) ? SignalMap[EventSignal] : EBangoTriggerSignalType::DoNothing;
				break;
			}
			else if (EventSignal == EBangoEventSignalType::EventDeactivated)
			{
				SignalToPerform = (Algo::Count(MasterEventsRecentSignals, EventSignal) == MasterEventsRecentSignals.Num()) ? SignalMap[EventSignal] : EBangoTriggerSignalType::DoNothing;
				break;
			}
		}
		default:
		{
			break;
		}
	}

	if (SignalToPerform == EBangoTriggerSignalType::DoNothing)
	{
		return;
	}

	SendSignal(FBangoTriggerSignal(SignalToPerform, SignalInstigator));
}

void UBangoTrigger_EventTriggered::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	FVector Start = GetEventComponent()->GetComponentLocation();
	
	if (GetWorld()->IsPlayInEditor())
	{
		for (TSoftObjectPtr<UBangoEventComponent> EventComponent : MasterEvents)
		{
			if (!EventComponent.IsValid())
			{
				continue;
			}

			FVector End = EventComponent->GetComponentLocation();

			BangoUtility::DebugDraw::DebugDrawDashedLine(GetWorld(), Start, End, 50.0f, BangoColor::TriggerDebugColor);
		}
	
	}
	else
	{
		for (TSoftObjectPtr<AActor> Actor : MasterEventSources)
		{
			if (!Actor.IsValid())
			{
				continue;
			}

			FVector End = Actor->GetActorLocation();

			BangoUtility::DebugDraw::DebugDrawDashedLine(GetWorld(), Start, End, 50.0f, BangoColor::TriggerDebugColor);
		}
	}
}
