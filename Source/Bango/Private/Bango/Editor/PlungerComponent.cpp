#include "Bango/Editor/PlungerComponent.h"

#include "Bango/Editor/PlungerSceneProxy.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango.h"

UBangoPlungerComponent::UBangoPlungerComponent()
{
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);

	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

FPrimitiveSceneProxy* UBangoPlungerComponent::CreateSceneProxy()
{
	auto X = new FBangoPlungerSceneProxy(this);
	
	if (X != nullptr)
	{
		X->SetColor(FLinearColor(FMath::RandRange(0.0, 1.0),FMath::RandRange(0.0, 1.0),FMath::RandRange(0.0, 1.0),FMath::RandRange(0.0, 1.0)));
	}

	return X;
}

#if WITH_EDITOR
bool UBangoPlungerComponent::ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionBox(InSelBox, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}

bool UBangoPlungerComponent::ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionFrustum(InFrustum, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}

void UBangoPlungerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHiddenInGame(!GhostPepperGames::Bango::bShowEventsInGame);
	
}

FBoxSphereBounds UBangoPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}
#endif
