#include "Bango/Core/Event.h"

#include "Bango/Utility/BangoColor.h"
#include "Bango/Action.h"
#include "Bango/Core/EventComponent.h"
#include "Bango/Core/EventSignal.h"

#if WITH_EDITORONLY_DATA
// TODO FText
TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> UBangoEvent::BangoEventsShowFlag(TEXT("BangoEventsShowFlag"), true, EShowFlagGroup::SFG_Developer, FText(INVTEXT("Bango Events")));
#endif

UBangoEvent::UBangoEvent()
{
}

bool UBangoEvent::GetUsesActivateLimit()
{
	return bUseTriggerLimits;
}

uint32 UBangoEvent::GetActivateLimit()
{
	return ActivateLimit;
}

uint32 UBangoEvent::GetDeactivateLimit()
{
	return DeactivateLimit;
}

UObject* UBangoEvent::GetLastActivateInstigator()
{
	return LastActivateInstigator;
}

UObject* UBangoEvent::GetLastDeactivateInstigator()
{
	return LastDeactivateInstigator;
}

double UBangoEvent::GetLastActivateTime()
{
	return LastActivateTime;
}

double UBangoEvent::GetLastDeactivateTime()
{
	return LastDeactivateTime;
}

uint32 UBangoEvent::GetActivateCount()
{
	return ActivateCount;
}

uint32 UBangoEvent::GetDeactivateCount()
{
	return DeactivateCount;
}

bool UBangoEvent::GetIsExpired()
{
	checkNoEntry();
	return false;
}

void UBangoEvent::Init()
{
	ExpiryDelegate.AddDynamic(GetEventComponent(), &UBangoEventComponent::OnEventExpired);
}

void UBangoEvent::RegisterAction(UBangoAction* Action)
{
	EventSignal.AddDynamic(Action, &UBangoAction::HandleSignal);
}

void UBangoEvent::UnregisterAction(UBangoAction* Action)
{
	EventSignal.RemoveDynamic(Action, &UBangoAction::HandleSignal);
}

void UBangoEvent::RespondToTriggerSignal(UBangoTrigger* Trigger, FBangoEventSignal Signal)
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
			case EBangoEventSignalType::ActivateEvent:
			{
				Delay = ActivateDelay;
				TimerHandle = &DelayedActivateHandle;
				OppositeTimerHandle = &DelayedDeactivateHandle;
				break;
			}
			case EBangoEventSignalType::DeactivateEvent:
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
			case EBangoExistingSignalHandling::LetRun:
			{
				bBreakResponse = true;
				break;
			}
			case EBangoExistingSignalHandling::Restart:
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
			case EBangoOpposingSignalHandling::CancelOpposingAndContinue:
			{
				GetWorld()->GetTimerManager().ClearTimer(*OppositeTimerHandle);
				OppositeTimerHandle->Invalidate();
				break;
			}
			case EBangoOpposingSignalHandling::CancelOpposing:
			{
				GetWorld()->GetTimerManager().ClearTimer(*OppositeTimerHandle);
				OppositeTimerHandle->Invalidate();
				bBreakResponse = true;
				break;
			}
			case EBangoOpposingSignalHandling::IgnoreOpposing:
			{
				break;
			}
			case EBangoOpposingSignalHandling::Exclusive:
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

void UBangoEvent::RespondToTriggerSignalDeferred(UBangoTrigger* Trigger, FBangoEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoEventSignalType::ActivateEvent:
		{
			DelayedActivateHandle.Invalidate();
			break;
		}
		case EBangoEventSignalType::DeactivateEvent:
		{
			DelayedDeactivateHandle.Invalidate();
			break;
		}
		default:
		{
			return;
		}
	}	
	
	EBangoActionSignalType ActionSignal = RespondToTriggerSignal_Impl(Trigger, Signal);
	
	if (ActionSignal == EBangoActionSignalType::None)
	{
		return;
	}

	EventSignal.Broadcast(this, FBangoActionSignal(ActionSignal, Signal.Instigator));

	AddInstigatorRecord(Signal.Instigator, ActionSignal);
	
	if (GetIsExpired())
	{
		ExpiryDelegate.Broadcast(this);
	}

#if WITH_EDITOR
	OnStateChange.ExecuteIfBound();
#endif
}

EBangoActionSignalType UBangoEvent::RespondToTriggerSignal_Impl(UBangoTrigger* Trigger, FBangoEventSignal Signal)
{
	return EBangoActionSignalType::None;
}

void UBangoEvent::AddInstigatorRecord(UObject* Instigator, EBangoActionSignalType SignalType)
{	
	double CurrentTime = GetWorld()->GetTimeSeconds();
	InstigatorRecords.UpdateInstigatorRecord(Instigator, SignalType, CurrentTime);

	switch (SignalType)
	{
		case EBangoActionSignalType::StartAction:
		{
			LastActivateInstigator = Instigator;
			LastActivateTime = CurrentTime;
			ActivateCount++;
			
			break;
		}
		case EBangoActionSignalType::StopAction:
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

UBangoEventComponent* UBangoEvent::GetEventComponent()
{
	return Cast<UBangoEventComponent>(GetOuter());
}

AActor* UBangoEvent::GetActor()
{
	return GetEventComponent()->GetOwner();
}

#if WITH_EDITOR
FLinearColor UBangoEvent::GetDisplayBaseColor()
{
	return FColor::Magenta;
}

void UBangoEvent::ApplyColorEffects(FLinearColor& Color)
{
}

bool UBangoEvent::GetIsActive()
{
	return false;
}
#endif


#if WITH_EDITORONLY_DATA
void UBangoEvent::AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data)
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
