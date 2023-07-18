﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Puny/PunyEvent.h"

#include "Components/PrimitiveComponent.h"

#include "PunyPlungerComponent.generated.h"

UCLASS(Within=PunyEventComponent, meta = (BlueprintSpawnableComponent))
class UPunyPlungerComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UPunyPlungerComponent();
	
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
private:

	/**  */
	TWeakObjectPtr<UPunyEvent> Event;

	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------
public:
	/**  */
	void SetSourceEvent(UPunyEvent* InEvent);
	
	// API
	// ============================================================================================
protected:
	/**  */
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

public:
	/**  */
	bool ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;

	/**  */
	bool ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;
	
public:
	void BeginPlay() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

public:
	FLinearColor GetColor();
	
	bool GetIsPushed();

	void OnCvarChange();

	UPunyEventComponent* GetEventComponent();
};