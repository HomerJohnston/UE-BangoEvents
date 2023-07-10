#include "Bango/DefaultImpl/Actions/BangoAction_DebugLog.h"

#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Engine/Light.h"

void UBangoAction_DebugLog::ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator)
{
	FString* String = SignalMessageMap.Find(Signal);
	
	if (String)
	{
		if (String->IsEmpty())
		{
			UE_LOG(Bango, Log, TEXT("Event <%s> deactivated for instigator <%s>"), *GetEventName().ToString(), *GetInstigatorName());
		}
		else
		{
			UE_LOG(Bango, Log, TEXT("%s"), **String);
		}
	}
}

FText UBangoAction_DebugLog::GetEventName()
{
#if WITH_EDITOR
	return GetEvent()->GetDisplayName();
#else
	return FText::FromName(GetEvent()->GetFName());
#endif
}

FString UBangoAction_DebugLog::GetInstigatorName()
{
	FString InstigatorName;

	if (AActor* ActorInstigator = Cast<AActor>(Instigator))
	{
		InstigatorName = Instigator->GetName();
	}
	else
	{
		InstigatorName = Instigator->GetName();
	}
	
	return InstigatorName;
}
