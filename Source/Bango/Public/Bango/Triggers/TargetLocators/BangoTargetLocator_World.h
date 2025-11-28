#pragma once

#include "Base/BangoTargetLocatorBase.h"

#include "BangoTargetLocator_World.generated.h"

/** Danger: this locator could return LOTS of actors! This could be expensive. */
UCLASS()
class UBangoTargetLocator_World : public UBangoTargetLocatorBase
{
	GENERATED_BODY()

	void UpdateCandidateActors(FBangoTargetLocatorQueryParams Params) override;
};