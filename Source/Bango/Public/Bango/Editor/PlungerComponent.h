#pragma once

#include "Components/PrimitiveComponent.h"

#include "PlungerComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class UBangoPlungerComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

	// Settings ---------------------------------
public:
	const FLinearColor NormalColor		= FLinearColor(0.40f, 0.70f, 1.00f, 1.00f);
	const FLinearColor DelayedColor		= FLinearColor(0.75f, 0.62f, 0.15f, 1.00f);
	const FLinearColor FrozenColor		= FLinearColor(0.52f, 0.58f, 0.84f, 1.00f);
	const FLinearColor ExpiredColor		= FLinearColor(0.05f, 0.05f, 0.05f, 0.05f);
	const FLinearColor TriggeredColor	= FLinearColor(0.75f, 0.13f, 0.13f, 1.00f);
	
	const float HandleOffsetNormal = 0;
	const float HandleOffsetTriggered = -40;

	const float Size = 30;
	const bool bIsScreenSizeScaled = true;
	const float ScreenSize = 0.0025;

	// State ------------------------------------
protected:
	FLinearColor CurrentColor;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	double LastStartTime;
	
public:
	UBangoPlungerComponent();

	float GetHandleOffset() { return HandleOffsetNormal; }

#if WITH_EDITOR
	bool ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;

	bool ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;

	void BeginPlay() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
#endif
};