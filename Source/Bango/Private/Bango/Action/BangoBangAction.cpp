#include "Bango/Action/BangoBangAction.h"
#include "Bango/Event/BangoEvent.h"

void UBangoBangAction::Start(UObject* StartInstigator)
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

void UBangoBangAction::StartDelayed()
{	
	StartTimerHandle.Invalidate();
	StopTimerHandle.Invalidate();

	bRunning = true;

	//OnStart();
}