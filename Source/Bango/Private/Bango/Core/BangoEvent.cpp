#include "Bango/Core/BangoEvent.h"

#include "Bango/Utility/BangoColor.h"
#include "Bango/BangoAction.h"
#include "Bango/Core/BangoEventComponent.h"
#include "Bango/Core/BangoTriggerSignal.h"
#include "Bango/Utility/BangoLog.h"

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
	EventTriggeredDelegate.AddDynamic(GetEventComponent(), &UBangoEventComponent::OnEventTriggered);
}

void UBangoEvent::RegisterAction(UBangoAction* Action)
{
	if (!IsValid(Action))
	{
		UE_LOG(Bango, Warning, TEXT("UBangoEvent::RegisterAction was passed a null action, ignoring"));
		return;
	}
	
	EventTriggeredDelegate.AddDynamic(Action, &UBangoAction::HandleSignal);
}

void UBangoEvent::UnregisterAction(UBangoAction* Action)
{
	if (!IsValid(Action))
	{
		UE_LOG(Bango, Warning, TEXT("UBangoEvent::UnregisterAction was passed a null action, ignoring"));
		return;
	}
	
	EventTriggeredDelegate.RemoveDynamic(Action, &UBangoAction::HandleSignal);
}

void UBangoEvent::RespondToTriggerSignal(UBangoTrigger* Trigger, FBangoTriggerSignal Signal)
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
			case EBangoTriggerSignalType::ActivateEvent:
			{
				Delay = ActivateDelay;
				TimerHandle = &DelayedActivateHandle;
				OppositeTimerHandle = &DelayedDeactivateHandle;
				break;
			}
			case EBangoTriggerSignalType::DeactivateEvent:
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

	if (!ShouldRespondToTrigger(Signal.Type))
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

void UBangoEvent::RespondToTriggerSignalDeferred(UBangoTrigger* Trigger, FBangoTriggerSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoTriggerSignalType::ActivateEvent:
		{
			DelayedActivateHandle.Invalidate();
			break;
		}
		case EBangoTriggerSignalType::DeactivateEvent:
		{
			DelayedDeactivateHandle.Invalidate();
			break;
		}
		default:
		{
			return;
		}
	}	
	
	EBangoEventSignalType ActionSignal = RespondToTriggerSignal_Impl(Trigger, Signal);
	
	if (ActionSignal == EBangoEventSignalType::None)
	{
		return;
	}

	EventTriggeredDelegate.Broadcast(this, FBangoEventSignal(ActionSignal, Signal.Instigator));

	AddInstigatorRecord(Signal.Instigator, ActionSignal);
	
	if (GetIsExpired())
	{
		ExpiryDelegate.Broadcast(this);
	}

#if WITH_EDITOR
	OnStateChange.ExecuteIfBound();
#endif
}

EBangoEventSignalType UBangoEvent::RespondToTriggerSignal_Impl(UBangoTrigger* Trigger, FBangoTriggerSignal Signal)
{
	return EBangoEventSignalType::None;
}

void UBangoEvent::AddInstigatorRecord(UObject* Instigator, EBangoEventSignalType SignalType)
{	
	double CurrentTime = GetWorld()->GetTimeSeconds();
	InstigatorRecords.UpdateInstigatorRecord(Instigator, SignalType, CurrentTime);

	switch (SignalType)
	{
		case EBangoEventSignalType::EventActivated:
		{
			LastActivateInstigator = Instigator;
			LastActivateTime = CurrentTime;
			ActivateCount++;
			
			break;
		}
		case EBangoEventSignalType::EventDeactivated:
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

bool UBangoEvent::ShouldRespondToTrigger(EBangoTriggerSignalType TriggerSignalType)
{
	return true;
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
