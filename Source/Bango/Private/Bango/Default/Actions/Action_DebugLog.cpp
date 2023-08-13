// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Default/Actions/Action_DebugLog.h"

#include "Bango/Utility/Log.h"
#include "Bango/Core/ActionSignal.h"
#include "Bango/Core/EventComponent.h"

UBangoAction_DebugLog::UBangoAction_DebugLog()
{
}

void UBangoAction_DebugLog::OnStart_Implementation(UBangoEvent* Event, UObject* Instigator)
{
#if UE_BUILD_SHIPPING

#else
	Handle(bUseActivateMessage, ActivateMessage, EBangoActionSignalType::StartAction, Instigator);
#endif
}

void UBangoAction_DebugLog::OnStop_Implementation(UBangoEvent* Event, UObject* Instigator)
{
#if UE_BUILD_SHIPPING

#else
	Handle(bUseDeactivateMessage, DeactivateMessage, EBangoActionSignalType::StopAction, Instigator);
#endif
}

void UBangoAction_DebugLog::Handle(bool& bUseCustomMessage, FString& CustomMessage, EBangoActionSignalType SignalType, UObject* Instigator)
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

FString UBangoAction_DebugLog::GetDefaultMessage(EBangoActionSignalType SignalType, UObject* Instigator)
{
	FString ActionName = StaticEnum<EBangoActionSignalType>()->GetValueAsString(SignalType);

	return FString::Format(TEXT("Action <{Name}> called on <{Actor}}> for instigator <{Instigator}>"),
	{
		{ TEXT("Name"), ActionName },
		{ TEXT("Actor"), GetEventName().ToString() },
		{ TEXT("Instigator"), IsValid(Instigator) ? Instigator->GetName() : FString("NULL") }
	});
}
