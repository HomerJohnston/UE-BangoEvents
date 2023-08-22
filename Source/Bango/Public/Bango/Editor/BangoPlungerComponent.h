// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Components/PrimitiveComponent.h"
#include "Bango/Core/BangoEvent.h"

#include "BangoPlungerComponent.generated.h"

struct FBangoPlungerDynamicData
{
	FLinearColor BaseColor = FLinearColor::Black;

	double ActivationTime = -1;
	double DeactivationTime = -1;

	bool bIsDisabled = false;

	bool bIsFrozen = false;
	bool bIsExpired = false;
	bool bIsActive = false;
};

UCLASS()
class UBangoPlungerComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoPlungerComponent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
	public:

	/** Rendering settings */
	const bool bIsScreenSizeScaled = true;

	/**  */
	const float ScreenSize = 0.0025;

	/**  */
	const double RecentPushHandleCooldownTime = 0.2;
	
	// ============================================================================================
	// STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------
public:
	
	// API
	// ============================================================================================
protected:
	/**  */
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

#if WITH_EDITOR
public:
	/**  */
	bool ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;

	/**  */
	bool ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;
#endif
	
public:
	void BeginPlay() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	
#if WITH_EDITOR
public:
	FLinearColor GetColor();
	
	bool GetIsActive();
#endif

public:
	void OnCvarChange();

	UBangoEventComponent* GetEventComponent();

	UBangoEvent* GetEvent();

	void CreateRenderState_Concurrent(FRegisterComponentContext* Context) override;

	void SendRenderDynamicData_Concurrent() override;
};
