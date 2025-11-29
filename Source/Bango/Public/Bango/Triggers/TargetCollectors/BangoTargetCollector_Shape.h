#pragma once

#include "Base/BangoTargetCollector.h"

#include "BangoTargetCollector_Shape.generated.h"

class UBoxComponent;
/** Danger: this locator could return LOTS of actors! This could be expensive. */
UCLASS()
class UBangoTargetCollector_Shape : public UBangoTargetCollector
{
	GENERATED_BODY()

public:
	UBangoTargetCollector_Shape();
	
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
	
	void UpdateCandidateActors(FBangoTargetCollectorQueryParams Params) override;
};