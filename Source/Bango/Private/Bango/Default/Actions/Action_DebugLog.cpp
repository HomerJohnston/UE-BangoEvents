﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Default/Actions/Action_DebugLog.h"

#include "Bango/Utility/Log.h"
#include "Bango/Core/ActionSignal.h"
#include "Bango/Core/EventComponent.h"

UBangoAction_DebugLog::UBangoAction_DebugLog()
{
}

void UBangoAction_DebugLog::HandleSignal_Implementation(UBangoEvent* Event, FBangoActionSignal Signal)
{
	FString Message;

	switch(Signal.Type)
	{
		case EBangoActionSignalType::StartAction:
		{
			if (bUseActivateMessage)
			{
				Message = ActivateMessage;
			}
			else
			{
				Message = GetDefaultMessage(Signal);
			}
			break;
		}
		case EBangoActionSignalType::StopAction:
		{
			if (bUseDeactivateMessage)
			{
				Message = DeactivateMessage;
			}
			else
			{
				Message = GetDefaultMessage(Signal);
			}
			break;
		}
		default:
		{
			Message = "UBangoAction_DebugLog - ERROR";
			break;
		}
	}

	if (PrintTo != EBangoAction_DebugLog_PrintTo::Screen)
	{
		UE_LOG(Bango, Display, TEXT("%s"), *Message);
	}

	if (PrintTo != EBangoAction_DebugLog_PrintTo::Log)
	{
		GEngine->AddOnScreenDebugMessage(OnScreenKey, OnScreenDisplayTime, OnScreenColor, Message);
	}
}

FText UBangoAction_DebugLog::GetEventName()
{
#if WITH_EDITOR
	return GetEventComponent()->GetDisplayName();
#else
	return FText::FromName(GetEvent()->GetFName());
#endif
}

FString UBangoAction_DebugLog::GetDefaultMessage(FBangoActionSignal Signal)
{
	FString ActionName = StaticEnum<EBangoActionSignalType>()->GetValueAsString(Signal.Type);

	return FString::Format(TEXT("Action <{Name}> called on <{Actor}}> for instigator <{Instigator}>"),
	{
		{ TEXT("Name"), ActionName },
		{ TEXT("Actor"), GetEventName().ToString() },
		{ TEXT("Instigator"), IsValid(Signal.Instigator) ? Signal.Instigator->GetName() : FString("NULL") }
	});
}
