﻿#include "Bango/Editor/PlungerComponent.h"

#include "Bango/Editor/PlungerSceneProxy.h"

UBangoPlungerComponent::UBangoPlungerComponent()
{
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);

	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

FPrimitiveSceneProxy* UBangoPlungerComponent::CreateSceneProxy()
{
	return new FBangoPlungerSceneProxy(this);
}

bool UBangoPlungerComponent::ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionBox(InSelBox, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}

bool UBangoPlungerComponent::ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionFrustum(InFrustum, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}

FBoxSphereBounds UBangoPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}
