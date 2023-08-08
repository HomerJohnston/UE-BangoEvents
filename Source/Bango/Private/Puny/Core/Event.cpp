#include "Puny/Core/Event.h"

#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/Log.h"
#include "Puny/Action.h"
#include "Puny/Action.h"
#include "Puny/Core/EventComponent.h"
#include "Puny/Core/EventSignal.h"
#include "Puny/Core/TriggerSignal.h"

#if WITH_EDITORONLY_DATA
// TODO FText
TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> UPunyEvent::PunyEventsShowFlag(TEXT("PunyEventsShowFlag"), true, EShowFlagGroup::SFG_Developer, FText(INVTEXT("Puny Events")));
#endif

UPunyEvent::UPunyEvent()
{
}

bool UPunyEvent::GetUsesActivateLimit()
{
	return bUseTriggerLimits;
}

uint32 UPunyEvent::GetActivateLimit()
{
	return ActivateLimit;
}

uint32 UPunyEvent::GetDeactivateLimit()
{
	return DeactivateLimit;
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

	float Delay = 0.0;

	FTimerHandle* TimerHandle = nullptr;
	FTimerHandle* OppositeTimerHandle = nullptr;
	
	if (bUseSignalDelays)
	{
		switch (Signal.Type)
		{
			case EPunyTriggerSignalType::ActivateEvent:
			{
				Delay = ActivateDelay;
				TimerHandle = &DelayedActivateHandle;
				OppositeTimerHandle = &DelayedDeactivateHandle;
				break;
			}
			case EPunyTriggerSignalType::DeactivateEvent:
			{
				Delay = DeactivateDelay;
				TimerHandle = &DelayedDeactivateHandle;
				OppositeTimerHandle = &DelayedActivateHandle;
				break;
			}
			default:
			{
				return;
			}
		}
	}

	bool bBreakResponse = false;

	if (TimerHandle && TimerHandle->IsValid())
	{
		switch (ExistingSignalHandling)
		{
			case EPunyExistingSignalHandling::LetRun:
			{
				bBreakResponse = true;
				break;
			}
			case EPunyExistingSignalHandling::Restart:
			{
				GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
				TimerHandle->Invalidate();
				break;
			}
			default:
			{
				bBreakResponse = true;
				checkNoEntry();
			}
		}
		
		if (bBreakResponse)
		{
			return;
		}
	}
	
	if (OppositeTimerHandle && OppositeTimerHandle->IsValid())
	{
		switch (OpposingSignalHandling)
		{
			case EPunyOpposingSignalHandling::CancelOpposingAndContinue:
			{
				GetWorld()->GetTimerManager().ClearTimer(*OppositeTimerHandle);
				OppositeTimerHandle->Invalidate();
				break;
			}
			case EPunyOpposingSignalHandling::CancelOpposing:
			{
				GetWorld()->GetTimerManager().ClearTimer(*OppositeTimerHandle);
				OppositeTimerHandle->Invalidate();
				bBreakResponse = true;
				break;
			}
			case EPunyOpposingSignalHandling::IgnoreOpposing:
			{
				break;
			}
			case EPunyOpposingSignalHandling::Exclusive:
			{
				bBreakResponse = true;
				break;
			}
			default:
			{
				bBreakResponse = true;
				checkNoEntry();
			}
		}
	}
	
	if (bBreakResponse)
	{
		return;
	}

	if (Delay > 0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::RespondToTriggerSignalDeferred, Trigger, Signal);
		GetWorld()->GetTimerManager().SetTimer(*TimerHandle, Delegate, Delay, false);
	}
	else
	{
		RespondToTriggerSignalDeferred(Trigger, Signal);
	}
}

void UPunyEvent::RespondToTriggerSignalDeferred(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	switch (Signal.Type)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{
			DelayedActivateHandle.Invalidate();
			break;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			DelayedDeactivateHandle.Invalidate();
			break;
		}
		default:
		{
			return;
		}
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
	
	OnStateChange.ExecuteIfBound();
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

#if WITH_EDITOR
FLinearColor UPunyEvent::GetDisplayBaseColor()
{
	return FColor::Magenta;
}

void UPunyEvent::ApplyColorEffects(FLinearColor& Color)
{
}

bool UPunyEvent::GetIsActive()
{
	return false;
}
#endif


#if WITH_EDITORONLY_DATA
void UPunyEvent::AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data)
{
	if (bUseSignalDelays)
	{
		if (DelayedActivateHandle.IsValid())
		{
			float TimeLeft = GetWorld()->GetTimerManager().GetTimerRemaining(DelayedActivateHandle);

			if (TimeLeft > 0)
			{
				Data.Add(FBangoDebugTextEntry("Activation delay:", FString::Printf(TEXT("%.1f"), TimeLeft), BangoColor::GreenBase));
			}
		}

		if (DelayedDeactivateHandle.IsValid())
		{
			float TimeLeft = GetWorld()->GetTimerManager().GetTimerRemaining(DelayedDeactivateHandle);

			if (TimeLeft > 0)
			{
				Data.Add(FBangoDebugTextEntry("Deactivation delay:", FString::Printf(TEXT("%.1f"), TimeLeft), BangoColor::GreenBase));
			}
		}
	}
}
#endif
