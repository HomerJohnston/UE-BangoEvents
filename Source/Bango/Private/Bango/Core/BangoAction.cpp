// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

void UBangoAction::StartInternal(ABangoEvent* EventActor, UObject* NewInstigator, double EventDelay)
{
	Event = EventActor;
	Instigator = NewInstigator;

	check(Event);
	check(Instigator);
	
	double RunDelay = Delay;

	if (!bIgnoreEventDelay)
	{
		RunDelay += EventDelay;
	}
	
	if (RunDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::PerformStart);
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, Delegate, RunDelay, false);
	}
	else
	{
		PerformStart();
	}
}

void UBangoAction::PerformStart()
{
	// If this was delayed, it's possible that the Instigator has become invalid. We will kill off this action if this happens and log a warning.
	if (!IsValid(Instigator) || !IsValid(Event))
	{
		return;
	}
	
	Start();
}

void UBangoAction::StopInternal(double EventDelay)
{
	double RunDelay = Delay;

	if (!bIgnoreEventDelay)
	{
		RunDelay += EventDelay;
	}
	
	if (RunDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::PerformStop);
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, Delegate, RunDelay, false);
	}
	else
	{
		PerformStop();
	}
}

void UBangoAction::PerformStop()
{
	if (!IsValid(Instigator) || !IsValid(Event))
	{
		return;
	}

	Stop();
}

void UBangoAction::Start_Implementation()
{
}

void UBangoAction::Stop_Implementation()
{
}

FText UBangoAction::GetDisplayName_Implementation()
{
	return DisplayName;
}
