#include "Bango/Triggers/TargetCollectors/BangoTargetCollector_Volume.h"

#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "Bango"

UBangoTargetCollector_Volume::UBangoTargetCollector_Volume()
{
	DisplayName = LOCTEXT("BangoTargetCollector_Volume_Name", "Volume");
	
	SortOrder = 20;
}

void UBangoTargetCollector_Volume::Setup()
{
	
}

void UBangoTargetCollector_Volume::UpdateCandidateActors(FBangoTargetCollectorQueryParams Params)
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

#undef LOCTEXT_NAMESPACE