#include "Bango/Action/BangoAction_Toggle.h"
#include "Bango/Event/BangoEvent.h"

void UBangoAction_Toggle::Start(UObject* StartInstigator)
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

void UBangoAction_Toggle::StartDelayed()
{
	StartTimerHandle.Invalidate();
	StopTimerHandle.Invalidate();

	bRunning = true;

	//OnStart();
}

void UBangoAction_Toggle::Stop(UObject* StopInstigator)
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

void UBangoAction_Toggle::StopDelayed()
{
	StopTimerHandle.Invalidate();
	StartTimerHandle.Invalidate();
	
	bRunning = false;

	//OnStop();
}
