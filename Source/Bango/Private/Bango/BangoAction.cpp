#include "Bango/BangoAction.h"

#include "Bango/Core/BangoEventSignal.h"
#include "Bango/Core/BangoEventComponent.h"

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Utility/BangoLog.h"

#if DO_CHECK
// Used to check to see if a derived class actually implemented GetWorld() or not
thread_local bool bStartDescriptionOverridden = false;
thread_local bool bStopDescriptionOverridden = false;
#endif

UBangoAction::UBangoAction()
{
	OnEventActivate = NAME_None;
	OnEventDeactivate = NAME_None;
}

void UBangoAction::BeginPlay_Implementation()
{
}

void UBangoAction::EndPlay_Implementation(const EEndPlayReason::Type EndPlayReason)
{
}

void UBangoAction::Initialize()
{
	if (OnEventActivate != NAME_None)
	{
		OnEventActivateDelegate.BindUFunction(this, OnEventActivate);
	}

	if (OnEventDeactivate != NAME_None)
	{
		OnEventDeactivateDelegate.BindUFunction(this, OnEventDeactivate);
	}
}

void UBangoAction::HandleSignal(UBangoEvent* Event, FBangoEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoEventSignalType::EventActivated:
		{
			OnEventActivateDelegate.Execute(Event, Signal.Instigator);
			//Handle(OnEventActivate, Event, Signal);
			break;
		}
		case EBangoEventSignalType::EventDeactivated:
		{
			OnEventDeactivateDelegate.Execute(Event, Signal.Instigator);
			//Handle(OnEventDeactivate, Event, Signal);
			break;
		}
		default:
		{
			break;
		}
	}
}

void UBangoAction::Handle(FName ActionFunction, UBangoEvent* Event, FBangoEventSignal Signal)
{
	
	/*
	switch (WhatToDo)
	{
		case EBangoActionRun::ExecuteStart:
		{
			Start(Event, Signal.Instigator);
			break;
		}
		case EBangoActionRun::ExecuteStop:
		{
			Stop(Event, Signal.Instigator);
			break;
		}
		default:
		{
			break;
		}
	}
	*/
}

UWorld* UBangoAction::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && ensureMsgf(GetOuter(), TEXT("BangoAction: %s has a null OuterPrivate in UBangoAction::GetWorld()"), *GetFullName())
		&& !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		return GetOuter()->GetWorld();
	}

	return nullptr;
}

UBangoEventComponent* UBangoAction::GetEventComponent() const
{
	return Cast<UBangoEventComponent>(GetOuter());
}

UBangoEvent* UBangoAction::GetEvent() const
{
	return GetEventComponent()->GetEvent();
}

AActor* UBangoAction::GetActor() const
{
	return GetEventComponent()->GetOwner();
}

#if WITH_EDITOR
FText UBangoAction::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}

void UBangoAction::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
}

bool UBangoAction::HasValidSetup()
{
	return true;
}

void UBangoAction::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
}

void UBangoAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif
