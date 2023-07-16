// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Trigger/BangoTrigger.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/Log.h"


UBangoTrigger::UBangoTrigger()
{
	BangoUtility::Signals::FillMap(SignalDelays, 0.0f);
}

ABangoEvent* UBangoTrigger::GetEventBP()
{
	return Cast<ABangoEvent>(GetOuter());
}

ABangoEvent* UBangoTrigger::GetEvent()
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoTrigger::SetEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		Enable();

		if (bUseSignalDelays && bReactToEventSignalling)
		{
			GetEvent()->OnEventSignalled.AddDynamic(this, &ThisClass::ReactToEventSignal);
		}
	}
	else
	{
		Disable();

		if (bUseSignalDelays && bReactToEventSignalling)
		{
			GetEvent()->OnEventSignalled.RemoveDynamic(this, &ThisClass::ReactToEventSignal);
		}
	}
}

void UBangoTrigger::Enable_Implementation()
{
	UE_LOG(Bango, Warning, TEXT("Trigger %s does not have any enable logic!"), *GetClass()->GetName());
}

void UBangoTrigger::Disable_Implementation()
{
	UE_LOG(Bango, Warning, TEXT("Trigger %s does not have any disable logic!"), *GetClass()->GetName());
}

void UBangoTrigger::SendTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	if (Signal == EBangoSignal::None) { return; }

	if (!PerformDelayedSignal(Signal, NewInstigator))
	{
		UE_LOG(Bango, Display, TEXT("Trigger %s sending %s to %s"), *GetName(), *StaticEnum<EBangoSignal>()->GetValueAsString(Signal), *GetEvent()->GetName());

		bIgnoreEventSignalling = true;
		TriggerSignal.Broadcast(Signal, NewInstigator);
		bIgnoreEventSignalling = false;
	}
}

bool UBangoTrigger::PerformDelayedSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	if (!bUseSignalDelays)
	{
		return false;
	}

	float* Delay = SignalDelays.Find(Signal);

	if (!Delay)
	{
		return false;
	}

	if (*Delay <= 0.0f)
	{
		return false;
	}

	FTimerHandle* Handle = DelayedSignalTimers.Find(Signal);

	if (Handle)
	{
		// A timer *might* be in progress, the only way to find out is to query the timer manager.
		float Time = GetWorld()->GetTimerManager().GetTimerRemaining(*Handle);

		if (Time >= 0 && !bAllowRestartingTimer)
		{
			return true;
		}
	}

	if (!Handle)
	{
		Handle = &DelayedSignalTimers.Add(Signal);
	}
	
	if (!CancelOpposingSignal(Signal))
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::SendTriggerSignal_Delayed, Signal, TWeakObjectPtr<UObject>(NewInstigator));
		GetWorld()->GetTimerManager().SetTimer(*Handle, Delegate, *Delay, false);
	}

	return true;
}

bool UBangoTrigger::CancelOpposingSignal(EBangoSignal Signal)
{
	if (!bCancelOpposingSignals) { return false; }

	EBangoSignal OpposingSignal = BangoUtility::Signals::GetOpposite(Signal);
	FTimerHandle* OtherHandle = DelayedSignalTimers.Find(OpposingSignal);

	if (OtherHandle)
	{
		GetWorld()->GetTimerManager().ClearTimer(*OtherHandle);
		DelayedSignalTimers.Remove(OpposingSignal);

		return true;
	}
	
	return false;
}

void UBangoTrigger::SendTriggerSignal_Delayed(EBangoSignal Signal, TWeakObjectPtr<UObject> NewInstigator)
{
	if (NewInstigator.IsValid())
	{
		UE_LOG(Bango, Display, TEXT("Trigger %s sending %s to %s (was delayed)"), *GetName(), *StaticEnum<EBangoSignal>()->GetValueAsString(Signal), *GetEvent()->GetName());

		bIgnoreEventSignalling = true;
		TriggerSignal.Broadcast(Signal, NewInstigator.Get());
		bIgnoreEventSignalling = false;
		
		DelayedSignalTimers.Remove(Signal);
	}
}

void UBangoTrigger::ReactToEventSignal(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator)
{
	//if (bIgnoreEventSignalling) { return; }
	
	UE_LOG(Bango, Display, TEXT("Trigger %s reacting to %s signal from %s"), *GetName(), *StaticEnum<EBangoSignal>()->GetValueAsString(Signal), *Event->GetName());

	if (Signal == EBangoSignal::None) { return; }

	if (bCancelOpposingSignals)
	{
		EBangoSignal OpposingSignal = BangoUtility::Signals::GetOpposite(Signal);
		FTimerHandle* OtherHandle = DelayedSignalTimers.Find(OpposingSignal);

		if (OtherHandle)
		{
			GetWorld()->GetTimerManager().ClearTimer(*OtherHandle);
			DelayedSignalTimers.Remove(OpposingSignal);
		}
	}
}

#if WITH_EDITOR
void UBangoTrigger::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}

void UBangoTrigger::DebugPrintTimeRemaining(TArray<FBangoDebugTextEntry>& Data, const FString& Label, EBangoSignal Signal)
{
	FTimerHandle* ActivateHandle = DelayedSignalTimers.Find(Signal);
		
	FNumberFormattingOptions NumberFormat;
	NumberFormat.MinimumFractionalDigits = 1;
	NumberFormat.MaximumFractionalDigits = 1;
	
	if (ActivateHandle && ActivateHandle->IsValid())
	{
		float Remaining = GetWorld()->GetTimerManager().GetTimerRemaining(*ActivateHandle);

		if (Remaining >= 0)
		{
			FLinearColor Color = Signal == EBangoSignal::Activate ? BangoColor::Green : BangoColor::Red;

			Data.Add(FBangoDebugTextEntry(Label, FText::AsNumber(Remaining, &NumberFormat).ToString(), BangoColor::Green));
		}
	}
}

void UBangoTrigger::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	if (bUseSignalDelays)
	{
		DebugPrintTimeRemaining(Data, "Activate", EBangoSignal::Activate);
		DebugPrintTimeRemaining(Data, "Deactivate", EBangoSignal::Deactivate);
	}
}

FText UBangoTrigger::GetDisplayName()
{
	return GetClass()->GetDisplayNameText();
}
#endif
