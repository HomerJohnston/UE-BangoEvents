// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Editor/PlungerComponent.h"

#include "Bango/Editor/PlungerSceneProxy.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/CVars.h"
#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

FLinearColor LightGrey			(0.50,	0.50,	0.50);
FLinearColor DarkGrey			(0.02,	0.02,	0.02);

FLinearColor Error				(1.00,	0.00,	1.00);

FLinearColor RedBase			(0.20,	0.00,	0.00);
FLinearColor OrangeBase			(0.15,	0.05,	0.00);
FLinearColor YellowBase			(0.10,	0.10,	0.00);
FLinearColor GreenBase			(0.00,	0.20,	0.00);
FLinearColor BlueBase			(0.00,	0.00,	0.20);

TMap<EBangoEventType, FLinearColor> ColorBaseMap
{
	{ EBangoEventType::Bang, RedBase },
	{ EBangoEventType::Toggle, GreenBase },
	{ EBangoEventType::Instanced, BlueBase },
};

FLinearColor BrightenColor(FLinearColor C)
{
	float M = 18.0f;
	float N = 0.40f;
	return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
}

FLinearColor LightDesatColor(FLinearColor C)
{
	float M = 0.40f;
	float N = 0.20f;
	return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
}

FLinearColor DarkDesatColor(FLinearColor C)
{
	float M = 0.10f;
	float N = 0.02f;
	return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
}

FLinearColor BangColor = RedBase;
FLinearColor ToggleColor = GreenBase;
FLinearColor InstancedColor = BlueBase;

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
	UWorld* World = GetWorld();

	ABangoEvent* Event = GetOwner<ABangoEvent>();
	check(Event);

	FLinearColor Color = Event->GetUsesCustomColor() ? Event->GetCustomColor() : ColorBaseMap[Event->GetType()];
	
	const FBangoEventStateFlag& State = Event->GetState();
	bool bToggles = Event->GetToggles();
	double LastHandleDownTime = Event->GetLastStartActionsTime();
	double LastHandleUpTime = Event->GetLastStopActionsTime();

	if (Event->GetType() >= EBangoEventType::MAX)
	{
		return Error;
	}
	
	if (World->IsGameWorld())
	{
		if (State.HasFlag(EBangoEventState::Active))
		{
			Color = BrightenColor(Color);
		}
		else if (State.HasFlag(EBangoEventState::Expired))
		{
			Color = DarkDesatColor(Color);
		}
		else if (State.HasFlag(EBangoEventState::Frozen))
		{
			Color = LightDesatColor(Color);
		}
		else if (State.HasFlag(EBangoEventState::Initialized))
		{
					
		}

		if (!bToggles)
		{
			FLinearColor ActiveColor = BrightenColor(Color);
			
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
		if (Event->GetStartsFrozen())
		{
			return LightDesatColor(Color);
		}

		return Color;
	}
	else
	{
		return Error;
	}
}
#endif

#if WITH_EDITOR
bool UBangoPlungerComponent::GetIsPlungerPushed()
{
	ABangoEvent* Event = Cast<ABangoEvent>(GetOwner());
	check(Event);

	switch (Event->GetType())
	{
		case EBangoEventType::Toggle:
		{
			return Event->HasCurrentState(EBangoEventState::Active);
		}
		default:
		{
			double Elapsed = GetWorld()->GetTimeSeconds() - Event->GetLastStartActionsTime();

			return Elapsed <= RecentPushHandleCooldownTime;
		}
	}
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
