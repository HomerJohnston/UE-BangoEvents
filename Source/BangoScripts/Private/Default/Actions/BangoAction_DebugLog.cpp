// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "BangoScripts/Default/Actions/BangoAction_DebugLog.h"

#include "BangoScripts/Core/BangoEvent.h"
#include "BangoScripts/Utility/BangoScriptsLog.h"
#include "BangoScripts/Core/BangoEventSignal.h"
#include "BangoScripts/Core/BangoEventComponent.h"
#include "Engine/Engine.h"

UBangoAction_DebugLog::UBangoAction_DebugLog()
{
#if WITH_EDITORONLY_DATA
	ActionFunctions.Add(GET_FUNCTION_NAME_CHECKED(ThisClass, PrintActivationMessage));
	ActionFunctions.Add(GET_FUNCTION_NAME_CHECKED(ThisClass, PrintDeactivationMessage));
#endif
	
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
		UE_LOG(LogBango, Display, TEXT("%s"), *Message);
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
