// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Editor/PlungerComponent.h"

#include "Bango/Editor/PlungerSceneProxy.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/CVars.h"
#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

UBangoPlungerComponent::UBangoPlungerComponent()
{
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);

	UPrimitiveComponent::SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

#if WITH_EDITOR
	FAutoConsoleVariableSink CVarSink(FConsoleCommandDelegate::CreateUObject(this, &ThisClass::OnCvarChange));
#endif
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

#if WITH_EDITOR
void UBangoPlungerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHiddenInGame(!GhostPepperGames::Bango::bShowEventsInGame);
}
#endif

#if WITH_EDITOR
FBoxSphereBounds UBangoPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}
#endif

#if WITH_EDITOR
FLinearColor UBangoPlungerComponent::GetColorForProxy()
{
	FLinearColor Color = FLinearColor::Black;

	UWorld* World = GetWorld();

	ABangoEvent* Event = Cast<ABangoEvent>(GetOwner());
	check(Event);

	const FBangoEventStateFlag& State = Event->GetState();
	bool bIsOnOff = Event->GetStartsAndStops();
	double LastHandleDownTime = Event->GetLastStartActionsTime();
	double LastHandleUpTime = Event->GetLastStartActionsTime();
	
	if (World->IsGameWorld())
	{
		if (State.HasFlag(EBangoEventState::Active))
		{
			Color = ActiveOnOffColor;
		}
		else if (State.HasFlag(EBangoEventState::Frozen) && State.HasFlag(EBangoEventState::Expired))
		{
			Color = FrozenExpiredColor;
		}
		else if (State.HasFlag(EBangoEventState::Frozen))
		{
			Color = FrozenColor;
		}
		else if (State.HasFlag(EBangoEventState::Expired))
		{
			Color = ExpiredColor;
		}
		else if (State.HasFlag(EBangoEventState::Initialized))
		{
			Color = bIsOnOff ? NormalOnOffColor : NormalColor;		
		}

		if (!bIsOnOff)
		{
			double ElapsedTimeSinceLastActivation = GetWorld()->GetTimeSeconds() - LastHandleDownTime;
			double Alpha = FMath::Clamp(ElapsedTimeSinceLastActivation / RecentPushColorCooldownTime, 0, 1);

			if (IsValid(GWorld) && (Alpha > 0))
			{
				Color = FMath::Lerp(ActiveColor, Color, Alpha);
			}
		}
		
		return Color;
	}
	else if (GetWorld()->IsEditorWorld())
	{
		return bIsOnOff ? NormalOnOffColor : NormalColor;
	}
	else
	{
		return FColor::Magenta;
	}
}
#endif

#if WITH_EDITOR
bool UBangoPlungerComponent::GetIsPlungerPushed()
{
	ABangoEvent* Event = Cast<ABangoEvent>(GetOwner());
	check(Event);
	
	double Elapsed = GetWorld()->GetTimeSeconds() - Event->GetLastStartActionsTime();

	return Elapsed <= RecentPushHandleCooldownTime;
}
#endif

#if WITH_EDITOR
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
#endif
