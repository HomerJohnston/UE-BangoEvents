// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Action/BangoAction.h"

#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Core/BangoInterfaces.h"

bool UBangoAction::GetUseStartDelay()
{
	return bUseStartDelay;
}

double UBangoAction::GetStartDelay()
{
	return StartDelay;
}

void UBangoAction::Start(UObject* StartInstigator)
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

void UBangoAction::StartDelayed()
{	
	StartTimerHandle.Invalidate();
	StopTimerHandle.Invalidate();

	bRunning = true;

	OnStart();
	/*
	if (this->Implements<UBangoBangEventActionInterface>())
	{
		IBangoBangEventActionInterface::Execute_OnStart(this);
	}
	else if (this->Implements<UBangoToggleEventActionInterface>())
	{
		IBangoToggleEventActionInterface::Execute_OnStart(this);
	}
	*/
}

void UBangoAction::OnStart_Implementation()
{
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
