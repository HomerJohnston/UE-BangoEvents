// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Core/BangoEvent.h"
#include "Components/PrimitiveComponent.h"

#include "PlungerComponent.generated.h"

struct FBangoEventStateFlag;
UCLASS(Within=BangoEvent, meta = (BlueprintSpawnableComponent))
class UBangoPlungerComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoPlungerComponent();
	
	// SETTINGS
	// ============================================================================================
public:

	// Rendering settings
	const bool bIsScreenSizeScaled = true;
	const float ScreenSize = 0.0025;

	/**  */
	const double RecentPushHandleCooldownTime = 0.2;
	const double RecentPushColorCooldownTime = 0.2;
	// STATE
	// ============================================================================================

	// API
	// ============================================================================================
protected:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

#if WITH_EDITOR
public:
	bool ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;

	bool ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;

	void BeginPlay() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	FLinearColor GetColorForProxy();
	
	bool GetIsPlungerPushed();

	void OnCvarChange();
#endif 
};