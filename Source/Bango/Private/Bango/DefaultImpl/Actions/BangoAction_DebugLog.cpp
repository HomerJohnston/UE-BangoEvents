#include "Bango/DefaultImpl/Actions/BangoAction_DebugLog.h"

#include "Bango/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Engine/Light.h"

void UBangoAction_DebugLog::OnStart_Implementation()
{
	if (ActivateMessage.IsEmpty())
	{
		UE_LOG(Bango, Log, TEXT("Event <%s> activated for instigator <%s>"), *GetEventName().ToString(), *GetInstigatorName());
	}
	else
	{
		UE_LOG(Bango, Log, TEXT("%s"), *ActivateMessage);
	}
}

void UBangoAction_DebugLog::OnStop_Implementation()
{
	if (DeactivateMessage.IsEmpty())
	{
		UE_LOG(Bango, Log, TEXT("Event <%s> deactivated for instigator <%s>"), *GetEventName().ToString(), *GetInstigatorName());
	}
	else
	{
		UE_LOG(Bango, Log, TEXT("%s"), *DeactivateMessage);
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
