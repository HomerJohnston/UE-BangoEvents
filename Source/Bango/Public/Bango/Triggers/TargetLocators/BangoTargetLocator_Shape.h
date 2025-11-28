#pragma once

#include "Base/BangoTargetLocatorBase.h"

#include "BangoTargetLocator_Shape.generated.h"

class UBoxComponent;
/** Danger: this locator could return LOTS of actors! This could be expensive. */
UCLASS()
class UBangoTargetLocator_Shape : public UBangoTargetLocatorBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<FVector> Points;
	
	/** How far above the trigger to extend the detection volume. */
	UPROPERTY()
	float Height;
	
	/** How far below the trigger to extend the detection volume. */
	UPROPERTY()
	float Drop;
	
protected:
	TWeakObjectPtr<UBoxComponent> BoxOverlapComponent;
	
protected:
	void Setup() override;
	
	void UpdateCandidateActors(FBangoTargetLocatorQueryParams Params) override;
};