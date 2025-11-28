#include "Bango/Triggers/TargetLocators/BangoTargetLocator_Volume.h"

#include "EngineUtils.h"

void UBangoTargetLocator_Volume::Setup()
{
	
}

void UBangoTargetLocator_Volume::UpdateCandidateActors(FBangoTargetLocatorQueryParams Params)
{
	UWorld* World = GetWorld();
	
	if (!IsValid(World))
	{
		CachedResult.Empty();
		return;
	}
	
	// I don't think there's any way to handle this intelligently. This is just an arbitrary number. Most triggers in most games will probably not have a lot of candidates.
	CachedResult.Reserve(20);

	
	for (AActor* Actor : TActorRange<AActor>(World, Params.ActorClass))
	{
		if (Params.IsValidMobility(Actor))
		{
			CachedResult.Add(Actor);
		}
	}
}
