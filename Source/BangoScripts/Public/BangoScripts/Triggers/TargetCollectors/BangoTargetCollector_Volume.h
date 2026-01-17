#pragma once

#include "Base/BangoTargetCollector.h"

#include "BangoTargetCollector_Volume.generated.h"

/** Danger: this locator could return LOTS of actors! This could be expensive. */
UCLASS()
class UBangoTargetCollector_Volume : public UBangoTargetCollector
{
	GENERATED_BODY()

public:
	UBangoTargetCollector_Volume();
	
protected:
	UPROPERTY()
	TArray<FOrientedBox> BoxVolumes;
	
	UPROPERTY()
	TArray<FSphere> SphereVolumes;
	
public:
	void Setup() override;
	
	void UpdateCandidateActors(FBangoTargetCollectorQueryParams Params) override;
};