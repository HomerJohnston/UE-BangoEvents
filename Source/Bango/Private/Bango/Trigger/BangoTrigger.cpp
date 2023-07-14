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
	}
	else
	{
		Disable();
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

	if (bUseSignalDelays)
	{
		float* Delay = SignalDelays.Find(Signal);

		if (Delay && *Delay >= 0.0f)
		{
			FTimerHandle& Handle = DelayedSignalTimers.FindOrAdd(Signal);
			
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

			FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::SendTriggerSignal_Delayed, Signal, TWeakObjectPtr<UObject>(NewInstigator));
			GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, *Delay, false);

			return;
		}
	}

	TriggerSignal.Broadcast(Signal, NewInstigator);
}

void UBangoTrigger::SendTriggerSignal_Delayed(EBangoSignal Signal, TWeakObjectPtr<UObject> NewInstigator)
{
	if (NewInstigator.IsValid())
	{
		TriggerSignal.Broadcast(Signal, NewInstigator.Get());
		DelayedSignalTimers.Remove(Signal);
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
