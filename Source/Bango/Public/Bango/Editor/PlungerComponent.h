﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

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
	
	// Mesh color settings
	const FLinearColor FrozenColor			{0.30, 0.35, 0.50, 1.00};
	const FLinearColor ExpiredColor			{0.05, 0.06, 0.07, 1.00};
	const FLinearColor FrozenExpiredColor	{0.01, 0.02, 0.03, 1.00};
	const FLinearColor ActiveColor			{0.40, 3.00, 0.40, 1.00};
	const FLinearColor ActiveOnOffColor		{0.30, 0.45, 3.00, 1.00};
	const FLinearColor NormalColor			{0.05, 0.20, 0.05, 1.00};
	const FLinearColor NormalOnOffColor		{0.05, 0.10, 0.40, 1.00};

	/**  */
	const double RecentPushHandleCooldownTime = 0.2;
	const double RecentPushColorCooldownTime = 0.4;
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