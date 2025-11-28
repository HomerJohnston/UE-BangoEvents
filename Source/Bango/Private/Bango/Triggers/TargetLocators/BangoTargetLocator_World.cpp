#include "Bango/Triggers/TargetLocators/BangoTargetLocator_World.h"

#include "EngineUtils.h"

void UBangoTargetLocator_World::UpdateCandidateActors(FBangoTargetLocatorQueryParams Params)
{
	UWorld* World = GetWorld();
	
	if (!IsValid(World))
	{
		CachedResult.Empty();
		return;
	}
	
	if (!IsValid(Params.ActorClass))
	{
		CachedResult.Reserve(World->GetActorCount());
	}
	else
	{
		// I don't think there's any way to handle this intelligently. This is just an arbitrary number. Most triggers in most games will probably not have a lot of candidates.
		CachedResult.Reserve(20);
	}
	
	for (AActor* Actor : TActorRange<AActor>(World, Params.ActorClass))
	{
		if (Params.IsValidMobility(Actor))
		{
			CachedResult.Add(Actor);
		}
	}
}
