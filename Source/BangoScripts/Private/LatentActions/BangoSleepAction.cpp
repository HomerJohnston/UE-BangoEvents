#include "BangoScripts/LatentActions/BangoSleepAction.h"

#define LOCTEXT_NAMESPACE "BangoScripts"

void FBangoSleepAction::UpdateOperation(FLatentResponse& Response)
{
	bool bIsSleepFinished = false;
	
	if (Duration >= 0.0f)
	{
		if (!bPaused)
		{
			TimeRemaining -= Response.ElapsedTime();
		}

		bIsSleepFinished = TimeRemaining <= 0.0f || bSkipped;
	}
	else
	{
		bIsSleepFinished = bSkipped;
	}

	bIsSleepFinished |= bCancelled;

	Response.FinishAndTriggerIf(bIsSleepFinished, ExecutionFunction, OutputLink, CallbackTarget);

	if (bIsSleepFinished)
	{
		if (!bCancelled)
		{
			OnComplete.Broadcast();
		}
	}
	else
	{
		OnTick.Broadcast();
	}
}

void FBangoSleepAction::NotifyActionAborted()
{
	bCancelled = true;
}

void FBangoSleepAction::Cancel()
{
	bCancelled = true;
	OnCancel.Broadcast();
}

void FBangoSleepAction::Skip()
{
	bSkipped = true;
	OnSkip.Broadcast();
}

void FBangoSleepAction::SetPaused(bool bInPaused)
{
	bPaused = bInPaused;
}

#if WITH_EDITOR
FString FBangoSleepAction::GetDescription() const
{
	static const FNumberFormattingOptions SleepTimeFormatOptions = FNumberFormattingOptions()
		.SetMinimumFractionalDigits(2)
		.SetMaximumFractionalDigits(2);
	
	return FText::Format(LOCTEXT("SleepActionTimeFmt", "{0} / {1}    "),
        FText::AsNumber(Duration - TimeRemaining, &SleepTimeFormatOptions),
	    FText::AsNumber(Duration, &SleepTimeFormatOptions)).ToString();
}
#endif

#undef LOCTEXT_NAMESPACE
