﻿#include "Bango/Action.h"

#include "Bango/Core/ActionSignal.h"
#include "Bango/Core/EventComponent.h"

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Utility/Log.h"

void UBangoAction::HandleSignal(UBangoEvent* Event, FBangoActionSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoActionSignalType::StartAction:
		{
			OnStart(Event, Signal.Instigator);
			break;
		}
		case EBangoActionSignalType::StopAction:
		{
			OnStop(Event, Signal.Instigator);
			break;
		}
		default:
		{
			break;
		}
	}
}

void UBangoAction::OnStart_Implementation(UBangoEvent* Event, UObject* Instigator)
{
	UE_LOG(Bango, Warning, TEXT("%s::OnStart Not implemented!"), *GetClass()->GetName());
}

void UBangoAction::OnStop_Implementation(UBangoEvent* Event, UObject* Instigator)
{
	UE_LOG(Bango, Warning, TEXT("%s::OnStop Not implemented!"), *GetClass()->GetName());
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

void UBangoAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif