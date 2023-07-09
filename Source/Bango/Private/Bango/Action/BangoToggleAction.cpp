#include "Bango/Action/BangoToggleAction.h"
#include "Bango/Event/BangoEvent.h"

bool UBangoToggleAction::GetUseStopDelay()
{
	return bUseStopDelay;
}

double UBangoToggleAction::GetStopDelay()
{
	return StopDelay;
}

void UBangoToggleAction::Stop(UObject* StopInstigator)
{
	if (bBlockFromStopping)
	{
		return;
	}
	
	if (StartTimerHandle.IsValid())
	{
		GetEvent()->GetWorldTimerManager().ClearTimer(StartTimerHandle);
		return;
	}

	if (bUseStopDelay && StopDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::StopDelayed);
		GetEvent()->GetWorldTimerManager().SetTimer(StopTimerHandle, Delegate, StopDelay, false);
	}
	else
	{
		StopDelayed();
	}
}

void UBangoToggleAction::OnStop_Implementation()
{
	// Default implementation
}

void UBangoToggleAction::StopDelayed()
{
	StopTimerHandle.Invalidate();
	StartTimerHandle.Invalidate();
	
	bRunning = false;

	OnStop();
}
