#pragma once

#include "Base/BangoTargetCollector.h"

#include "BangoTargetCollector_World.generated.h"

/** To prevent generating a massive targets array, this collector only works when an actor subclass is passed into the params. */
UCLASS()
class UBangoTargetCollector_World : public UBangoTargetCollector
{
	GENERATED_BODY()
	
public:
	UBangoTargetCollector_World();
	
	
	void Setup() override;
	
	void UpdateCandidateActors(FBangoTargetCollectorQueryParams Params) override;
};