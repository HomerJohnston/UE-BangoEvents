#include "Puny/PunyEvent.h"

#include "Puny/PunyAction.h"
#include "Puny/PunyEventComponent.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

#if WITH_EDITORONLY_DATA
// TODO FText
TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> UPunyEvent::PunyEventsShowFlag(TEXT("PunyEventsShowFlag"), true, EShowFlagGroup::SFG_Developer, FText(INVTEXT("Puny Events")));
#endif

UPunyEvent::UPunyEvent()
{
}

UObject* UPunyEvent::GetLastActivateInstigator()
{
	return LastActivateInstigator;
}

UObject* UPunyEvent::GetLastDeactivateInstigator()
{
	return LastDeactivateInstigator;
}

double UPunyEvent::GetLastActivateTime()
{
	return LastActivateTime;
}

double UPunyEvent::GetLastDeactivateTime()
{
	return LastDeactivateTime;
}

void UPunyEvent::Init()
{
}

void UPunyEvent::RegisterAction(UPunyAction* Action)
{
	EventSignal.AddDynamic(Action, &UPunyAction::HandleSignal);
}

void UPunyEvent::UnregisterAction(UPunyAction* Action)
{
	EventSignal.RemoveDynamic(Action, &UPunyAction::HandleSignal);
}

void UPunyEvent::RespondToTriggerSignal(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
}

void UPunyEvent::AddInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType)
{
	double CurrentTime = GetWorld()->GetTimeSeconds();
	InstigatorRecords.UpdateInstigatorRecord(Instigator, SignalType, CurrentTime);

	// TODO can this all be editor only?
	// TODO \/
	switch (SignalType)
	{
		case EPunyEventSignalType::StartAction:
		{
			LastActivateInstigator = Instigator;
			LastActivateTime = CurrentTime;
			break;
		}
		case EPunyEventSignalType::StopAction:
		{
			LastDeactivateInstigator = Instigator;
			LastDeactivateTime = CurrentTime;
			break;
		}
		default:
		{
			
		}
	}
	// TODO /\
	
}

UPunyEventComponent* UPunyEvent::GetEventComponent()
{
	return Cast<UPunyEventComponent>(GetOuter());
}

AActor* UPunyEvent::GetActor()
{
	return GetEventComponent()->GetOwner();
}

FLinearColor UPunyEvent::GetDisplayColor()
{
	return FColor::Magenta;
}
