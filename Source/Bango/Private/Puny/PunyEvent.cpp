#include "Puny/PunyEvent.h"

#include "Bango/Utility/Log.h"
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

bool UPunyEvent::GetUsesActivateLimit()
{
	return bUseActivateLimit;
}

uint32 UPunyEvent::GetActivateLimit()
{
	return ActivateLimit;
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

uint32 UPunyEvent::GetActivateCount()
{
	return ActivateCount;
}

uint32 UPunyEvent::GetDeactivateCount()
{
	return DeactivateCount;
}

bool UPunyEvent::GetIsExpired()
{
	checkNoEntry();
	return false;
}

void UPunyEvent::Init()
{
	ExpiryDelegate.AddDynamic(GetEventComponent(), &UPunyEventComponent::OnEventExpired);
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
	if (GetIsExpired())
	{
		return;
	}

	EPunyEventSignalType ActionSignal = RespondToTriggerSignal_Impl(Trigger, Signal);
	
	if (ActionSignal == EPunyEventSignalType::None)
	{
		return;
	}

	EventSignal.Broadcast(this, FPunyEventSignal(ActionSignal, Signal.Instigator));

	AddInstigatorRecord(Signal.Instigator, ActionSignal);
	
	if (GetIsExpired())
	{
		ExpiryDelegate.Broadcast(this);
	}
}

EPunyEventSignalType UPunyEvent::RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	return EPunyEventSignalType::None;
}

void UPunyEvent::AddInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType)
{	
	double CurrentTime = GetWorld()->GetTimeSeconds();
	InstigatorRecords.UpdateInstigatorRecord(Instigator, SignalType, CurrentTime);

	switch (SignalType)
	{
		case EPunyEventSignalType::StartAction:
		{
			LastActivateInstigator = Instigator;
			LastActivateTime = CurrentTime;
			ActivateCount++;
			break;
		}
		case EPunyEventSignalType::StopAction:
		{
			LastDeactivateInstigator = Instigator;
			LastDeactivateTime = CurrentTime;
			DeactivateCount++;
			break;
		}
		default:
		{
			
		}
	}
}

UPunyEventComponent* UPunyEvent::GetEventComponent()
{
	return Cast<UPunyEventComponent>(GetOuter());
}

AActor* UPunyEvent::GetActor()
{
	return GetEventComponent()->GetOwner();
}

FLinearColor UPunyEvent::GetDisplayBaseColor()
{
	return FColor::Magenta;
}

void UPunyEvent::ApplyColorEffects(FLinearColor& Color)
{
}

#if WITH_EDITORONLY_DATA
bool UPunyEvent::GetIsPlungerPushed()
{
	return false;
}

#endif
