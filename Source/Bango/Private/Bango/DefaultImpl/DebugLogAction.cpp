#include "Bango/DefaultImpl/DebugLogAction.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"
#include "Engine/Light.h"

void UBangoAction_DebugLog::Start_Implementation()
{
	if (StartText.IsEmpty())
	{		
		UE_LOG(Bango, Log, TEXT("Event <%s> started for instigator <%s>"), *GetEventName(), *GetInstigatorName());
	}
	else
	{
		UE_LOG(Bango, Log, TEXT("%s"), *StartText);
	}
}

void UBangoAction_DebugLog::Stop_Implementation()
{
	if (StopText.IsEmpty())
	{
		UE_LOG(Bango, Log, TEXT("Event <%s> stopped for instigator <%s>"), *GetEventName(), *GetInstigatorName());
	}
	else
	{
		UE_LOG(Bango, Log, TEXT("%s"), *StopText);
	}

}

FString UBangoAction_DebugLog::GetEventName()
{
	return Event->GetName();
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
