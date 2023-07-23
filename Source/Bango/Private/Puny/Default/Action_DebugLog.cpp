#include "Puny/Default/Action_DebugLog.h"

#include "Bango/Utility/Log.h"
#include "Puny/Core/EventSignal.h"
#include "Puny/Core/EventComponent.h"

UPunyAction_DebugLog::UPunyAction_DebugLog()
{
}

void UPunyAction_DebugLog::HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal)
{
	FString ActionName = StaticEnum<EPunyEventSignalType>()->GetValueAsString(Signal.Type);
	
	FString Message;

	switch(Signal.Type)
	{
		case EPunyEventSignalType::StartAction:
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
		case EPunyEventSignalType::StopAction:
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
			Message = "UPunyAction_DebugLog - ERROR";
			break;
		}
	}
	
	UE_LOG(Bango, Display, TEXT("%s"), *Message);
}

FText UPunyAction_DebugLog::GetEventName()
{
#if WITH_EDITOR
	return GetEventComponent()->GetDisplayName();
#else
	return FText::FromName(GetEvent()->GetFName());
#endif
}

FString UPunyAction_DebugLog::GetDefaultMessage(FPunyEventSignal Signal)
{
	FString ActionName = StaticEnum<EPunyEventSignalType>()->GetValueAsString(Signal.Type);

	return FString::Format(TEXT("Action <{Name}> called on <{Actor}}> for instigator <{Instigator}>"),
	{
		{ TEXT("Name"), ActionName },
		{ TEXT("Actor"), GetEventName().ToString() },
		{ TEXT("Instigator"), IsValid(Signal.Instigator) ? Signal.Instigator->GetName() : FString("NULL") }
	});
}
