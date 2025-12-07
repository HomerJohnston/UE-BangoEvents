#include "Bango/Triggers/TargetCollectors/BangoTargetCollector_World.h"

#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "Bango"

UBangoTargetCollector_World::UBangoTargetCollector_World()
{
#if WITH_EDITOR
	DisplayName = LOCTEXT("BangoTargetCollector_World_Name", "World");
	SortOrder = 30;
#endif
}

void UBangoTargetCollector_World::Setup()
{
	
}

void UBangoTargetCollector_World::UpdateCandidateActors(FBangoTargetCollectorQueryParams Params)
{
	UWorld* World = GetWorld();
	
	if (!IsValid(World))
	{
		CachedResult.Empty();
		return;
	}
	
	if (!IsValid(Params.ActorClass))
	{
		CachedResult.Empty();
		return;
	}

	// I don't think there's any way to handle this intelligently. This is just an arbitrary number. 
	// Most triggers in most games will probably not have a lot of candidates.
	CachedResult.Reserve(20);
	
	
	
	for (AActor* Actor : TActorRange<AActor>(World, Params.ActorClass))
	{
		if (Params.IsValidMobility(Actor))
		{
			CachedResult.Add(Actor);
		}
	}
}

#undef LOCTEXT_NAMESPACE