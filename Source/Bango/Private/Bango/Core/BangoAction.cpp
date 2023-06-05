// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

void UBangoAction::Start(ABangoEvent* EventActor, UObject* NewInstigator)
{
	Event = EventActor;
	Instigator = NewInstigator;

	check(Event);
	check(Instigator);

	if (bUseStartDelay && StartDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::StartDelayed);
		Event->GetWorldTimerManager().SetTimer(StartTimerHandle, Delegate, StartDelay, false);
	}
	else
	{
		OnStart();
	}
}

void UBangoAction::StartDelayed()
{
	StartTimerHandle.Invalidate();
	OnStart();
}

void UBangoAction::Stop()
{
	if (StartTimerHandle.IsValid())
	{
		Event->GetWorldTimerManager().ClearTimer(StartTimerHandle);
		return;
	}

	if (bUseStopDelay && StopDelay > 0.0)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::StopDelayed);
		Event->GetWorldTimerManager().SetTimer(StopTimerHandle, Delegate, StopDelay, false);
	}
	else
	{
		OnStop();
	}
}

void UBangoAction::StopDelayed()
{
	StopTimerHandle.Invalidate();
	OnStop();
}

void UBangoAction::OnStart_Implementation() { /* Placeholder */}

void UBangoAction::OnStop_Implementation() { /* Placeholder */ }

FString UBangoAction::GetDisplayName_Implementation()
{
	return DisplayName.ToString();
}
