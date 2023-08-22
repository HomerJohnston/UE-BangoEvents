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
	WhenEventActivates = EBangoActionRun::ExecuteStart;
	WhenEventDeactivates = EBangoActionRun::ExecuteStop;

#if WITH_EDITOR
	// TODO localization
	DoNothingDescription = "Do Nothing";
	StartDescription = "Execute Start";
	StopDescription = "Execute Stop";
#endif
}

void UBangoAction::BeginPlay_Implementation()
{
}

void UBangoAction::EndPlay_Implementation(const EEndPlayReason::Type EndPlayReason)
{
}

void UBangoAction::HandleSignal(UBangoEvent* Event, FBangoEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoEventSignalType::EventActivated:
		{
			Handle(WhenEventActivates, Event, Signal);
			break;
		}
		case EBangoEventSignalType::EventDeactivated:
		{
			Handle(WhenEventDeactivates, Event, Signal);
			break;
		}
		default:
		{
			break;
		}
	}
}

void UBangoAction::Handle(EBangoActionRun WhatToDo, UBangoEvent* Event, FBangoEventSignal Signal)
{
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
}

void UBangoAction::Start_Implementation(UBangoEvent* Event, UObject* Instigator)
{
	UE_LOG(Bango, Warning, TEXT("%s::Start called but not implemented!"), *GetClass()->GetName());
}

void UBangoAction::Stop_Implementation(UBangoEvent* Event, UObject* Instigator)
{
	UE_LOG(Bango, Warning, TEXT("%s::Stop called but not implemented!"), *GetClass()->GetName());
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

EBangoActionRun UBangoAction::LookupSettingForDescription(TSharedPtr<FString> Description) const
{
	if (Description->Equals(StartDescription))
	{
		return EBangoActionRun::ExecuteStart;
	}

	if (Description->Equals(StopDescription))
	{
		return EBangoActionRun::ExecuteStop;
	}

	return EBangoActionRun::DoNothing;
}

const FString& UBangoAction::GetDescriptionFor(EBangoActionRun ActionRun) const
{
	switch (ActionRun)
	{
		case EBangoActionRun::ExecuteStart:
		{
			return StartDescription;
		}
		case EBangoActionRun::ExecuteStop:
		{
			return StopDescription;
		}
		case EBangoActionRun::DoNothing:
		{
			return DoNothingDescription;
		}
	}

	return DoNothingDescription;
}

void UBangoAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif