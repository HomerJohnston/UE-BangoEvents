// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

void UBangoAction::RunInternal(ABangoEvent* EventActor, UObject* Instigator, double EventDelay)
{
	double RunDelay = Delay;

	if (!bIgnoreEventDelay)
	{
		RunDelay += EventDelay;
	}
	
	if (RunDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::PerformRun, EventActor, Instigator);
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, Delegate, RunDelay, false);
	}
	else
	{
		PerformRun(EventActor, Instigator);
	}
}

void UBangoAction::PerformRun(ABangoEvent* EventActor, UObject* Instigator)
{
	// If this was delayed, it's possible that the Instigator has become invalid. We will kill off this action if this happens and log a warning.
	if (!IsValid(Instigator))
	{
		UE_LOG(Bango, Warning, TEXT("PerformRun aborting - Instigator was null"));
		OnRunFailed.Broadcast();
		OnFailToRun(EventActor);
		return;
	}
	
	OnRunEvent.Broadcast(Instigator);
	Run(EventActor, Instigator);
}

FText UBangoAction::GetDisplayName_Implementation()
{
	return DisplayName;
}

void UBangoAction::OnFailToRun_Implementation(ABangoEvent* EventActor)
{
}

void UBangoAction::Run_Implementation(ABangoEvent* EventActor, UObject* Instigator)
{
}
