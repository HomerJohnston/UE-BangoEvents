#pragma once

#include "Base/BangoTargetLocatorBase.h"

#include "BangoTargetLocator_Volume.generated.h"

/** Danger: this locator could return LOTS of actors! This could be expensive. */
UCLASS()
class UBangoTargetLocator_Volume : public UBangoTargetLocatorBase
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FOrientedBox> BoxVolumes;
	
	UPROPERTY()
	TArray<FSphere> SphereVolumes;
	
	void Setup() override;
	
	void UpdateCandidateActors(FBangoTargetLocatorQueryParams Params) override;
};