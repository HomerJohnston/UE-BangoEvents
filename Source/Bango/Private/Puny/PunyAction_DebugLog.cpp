#include "Puny/PunyAction_DebugLog.h"

#include "Bango/Utility/Log.h"
#include "Puny/PunyEventSignal.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyEventComponent.h"

UPunyAction_DebugLog::UPunyAction_DebugLog()
{
	SignalMessages = { {EPunyEventSignalType::StartAction, ""}, {EPunyEventSignalType::StopAction, ""} };
}

void UPunyAction_DebugLog::HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal)
{
	if (FString* Log = SignalMessages.Find(Signal.Type))
	{
		if (Log->IsEmpty())
		{
			FString ActionName = StaticEnum<EPunyEventSignalType>()->GetValueAsString(Signal.Type);
			
			UE_LOG(Bango, Log, TEXT("Action <%s> called on <%s> for instigator <%s>"), *ActionName, *GetEventName().ToString(), *(IsValid(Signal.Instigator) ? *Signal.Instigator->GetName() : FString("NULL")));
		}
		else
		{
			UE_LOG(Bango, Display, TEXT("%s"), **Log);
		}
	}
}

FText UPunyAction_DebugLog::GetEventName()
{
#if WITH_EDITOR
	return GetEvent()->GetDisplayName();
#else
	return FText::FromName(GetEvent()->GetFName());
#endif
}
