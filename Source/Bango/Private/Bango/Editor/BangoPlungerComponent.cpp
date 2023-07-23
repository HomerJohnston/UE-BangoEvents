// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Editor/BangoPlungerComponent.h"
#include "Bango/Core/BangoSignal.h"
#include "Bango/Editor/BangoPlungerSceneProxy.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Event/BangoEvent.h"

UBangoPlungerComponent::UBangoPlungerComponent()
{
	bIsEditorOnly = true;
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);

	UPrimitiveComponent::SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	FAutoConsoleVariableSink CVarSink(FConsoleCommandDelegate::CreateUObject(this, &ThisClass::OnCvarChange));
}

FPrimitiveSceneProxy* UBangoPlungerComponent::CreateSceneProxy()
{
	return new FBangoPlungerSceneProxy(this);
}

#if WITH_EDITOR
bool UBangoPlungerComponent::ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionBox(InSelBox, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}
#endif

#if WITH_EDITOR
bool UBangoPlungerComponent::ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionFrustum(InFrustum, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}
#endif

void UBangoPlungerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>(); 
	
	SetHiddenInGame(!DevSettings->GetShowEventsInGame());
}

FBoxSphereBounds UBangoPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}

#if WITH_EDITOR
FLinearColor UBangoPlungerComponent::GetColorForProxy()
{
	ABangoEvent* Event = GetOwner<ABangoEvent>();

	if (IsValid(Event))
	{
		return Event->GetColorForProxy();
	}

	return FLinearColor::Black;
}
#endif

bool UBangoPlungerComponent::GetIsPlungerPushed()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return false;
	}
	
	ABangoEvent* Event = Cast<ABangoEvent>(GetOwner());
	check(Event);
	
	return Event->HasCurrentState(EBangoEventState::Active) || ((GetWorld()->GetTimeSeconds() - Event->GetLastTriggerTime(EBangoSignal::Activate)) <= RecentPushHandleCooldownTime);
#else
	return false;
#endif
}

void UBangoPlungerComponent::OnCvarChange()
{
	AActor* OwnerActor = GetOwner();

	if (!IsValid(OwnerActor))
	{
		return;
	}

	UWorld* World = OwnerActor->GetWorld();
	
	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	const IConsoleVariable* ShowInGameCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));

	bool bNewHiddenInGame = !ShowInGameCVar->GetBool();
	
	SetHiddenInGame(bNewHiddenInGame);
}

bool UBangoPlungerComponent::GetEventHasCustomMesh()
{
	return false;
	//return GetOuterABangoEvent()->GetUsesCustomMesh(); // WITHIN specifier on uclass
}
