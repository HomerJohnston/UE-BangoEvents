﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Default/Actions/BangoAction_DebugLog.h"

#include "Bango/Core/BangoEvent.h"
#include "Bango/Utility/BangoLog.h"
#include "Bango/Core/BangoEventSignal.h"
#include "Bango/Core/BangoEventComponent.h"

UBangoAction_DebugLog::UBangoAction_DebugLog()
{
	ActionFunctions.Add(GET_FUNCTION_NAME_CHECKED(ThisClass, PrintActivationMessage));
	ActionFunctions.Add(GET_FUNCTION_NAME_CHECKED(ThisClass, PrintDeactivationMessage));

	OnEventActivate = GET_FUNCTION_NAME_CHECKED(ThisClass, PrintActivationMessage);
	OnEventDeactivate = GET_FUNCTION_NAME_CHECKED(ThisClass, PrintDeactivationMessage);
}

void UBangoAction_DebugLog::PrintActivationMessage(UBangoEvent* Event, UObject* Instigator)
{
#if UE_BUILD_SHIPPING
	
#else
	Handle(bOverrideActivateMessage, ActivateMessage, EBangoEventSignalType::EventActivated, Instigator);
#endif
}

void UBangoAction_DebugLog::PrintDeactivationMessage(UBangoEvent* Event, UObject* Instigator)
{
#if UE_BUILD_SHIPPING
	
#else
	Handle(bOverrideDeactivateMessage, DeactivateMessage, EBangoEventSignalType::EventDeactivated, Instigator);
#endif
}

void UBangoAction_DebugLog::Handle(bool& bUseCustomMessage, FString& CustomMessage, EBangoEventSignalType SignalType, UObject* Instigator)
{
	if (bUseCustomMessage)
	{
		Print(CustomMessage);
	}
	else
	{
		FString DefaultMessage = GetDefaultMessage(SignalType, Instigator);

		Print(DefaultMessage);
	}
}

void UBangoAction_DebugLog::Print(FString& Message)
{
	if (PrintTo != EBangoAction_DebugLog_PrintTo::Screen)
	{
		UE_LOG(Bango, Display, TEXT("%s"), *Message);
	}

	if (PrintTo != EBangoAction_DebugLog_PrintTo::Log)
	{
		GEngine->AddOnScreenDebugMessage(OnScreenKey, OnScreenDisplayTime, OnScreenColor, *Message);
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

FString UBangoAction_DebugLog::GetDefaultMessage(EBangoEventSignalType SignalType, UObject* Instigator)
{
	FString ActionName = StaticEnum<EBangoEventSignalType>()->GetValueAsString(SignalType);

	return FString::Format(TEXT("Action <{Name}> called on <{Actor}}> for instigator <{Instigator}>"),
	{
		{ TEXT("Name"), ActionName },
		{ TEXT("Actor"), GetEventName().ToString() },
		{ TEXT("Instigator"), IsValid(Instigator) ? Instigator->GetName() : FString("NULL") }
	});
}
