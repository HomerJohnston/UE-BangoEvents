#include "Bango/Action/BangoToggleAction.h"
#include "Bango/Event/BangoEvent.h"

void UBangoToggleAction::Start(UObject* StartInstigator)
{	
	if (bBlockFromStarting)
	{
		return;
	}
	
	Instigator = StartInstigator;

	check(Instigator);

	if (bUseStartDelay && StartDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::StartDelayed);
		GetEvent()->GetWorldTimerManager().SetTimer(StartTimerHandle, Delegate, StartDelay, false);
	}
	else
	{
		StartDelayed();
	}
}

void UBangoToggleAction::StartDelayed()
{
	StartTimerHandle.Invalidate();
	StopTimerHandle.Invalidate();

	bRunning = true;

	//OnStart();
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

void UBangoToggleAction::StopDelayed()
{
	StopTimerHandle.Invalidate();
	StartTimerHandle.Invalidate();
	
	bRunning = false;

	//OnStop();
}
