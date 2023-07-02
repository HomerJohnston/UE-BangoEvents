// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

bool UBangoAction::GetUseStartDelay()
{
	return bUseStartDelay;
}

bool UBangoAction::GetUseStopDelay()
{
	return bUseStopDelay;
}

double UBangoAction::GetStartDelay()
{
	return StartDelay;
}

double UBangoAction::GetStopDelay()
{
	return StopDelay;
}

void UBangoAction::Start(ABangoEvent* EventActor, UObject* NewInstigator)
{
	if (bBlockFromStarting)
	{
		return;
	}
	
	Instigator = NewInstigator;

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

void UBangoAction::StartDelayed()
{	
	StartTimerHandle.Invalidate();
	StopTimerHandle.Invalidate();

	bRunning = true;

	OnStart();
}

void UBangoAction::Stop()
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

void UBangoAction::StopDelayed()
{
	StopTimerHandle.Invalidate();
	StartTimerHandle.Invalidate();
	
	bRunning = false;
	
	OnStop();
}

UWorld* UBangoAction::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return GetOuter()->GetWorld();
	}
	else
	{
		return nullptr;		
	}
}

ABangoEvent* UBangoAction::GetEvent() const
{
	return Cast<ABangoEvent>(GetOuter());
}

#if WITH_EDITOR
void UBangoAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif

void UBangoAction::OnStart_Implementation() { /* Placeholder */}

void UBangoAction::OnStop_Implementation() { /* Placeholder */ }

FText UBangoAction::GetDisplayName_Implementation()
{
	if (DisplayName.IsEmpty())
	{
#if WITH_EDITOR
		return GetClass()->GetDisplayNameText();
#else
		return FText::FromString(GetClass()->GetName());
#endif
	}
	
	return DisplayName;
}
