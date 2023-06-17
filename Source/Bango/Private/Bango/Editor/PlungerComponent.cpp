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
//	{ EBangoEventType::Instanced, BlueBase },
};

FLinearColor BrightenColor(FLinearColor C)
{
	float M = 18.0f;
	float N = 0.40f;
	return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
}

FLinearColor EnhanceColor(FLinearColor C)
{
	float M = 2.0f;
	float N = -0.05f;
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

FLinearColor VeryDarkDesatColor(FLinearColor C)
{
	float M = 0.05f;
	float N = 0.01f;
	return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
}

FLinearColor BangColor = RedBase;
FLinearColor ToggleColor = GreenBase;
FLinearColor InstancedColor = BlueBase;

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
	
	SetHiddenInGame(!DevSettings->bShowEventsInGame);
}

FBoxSphereBounds UBangoPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}

FLinearColor UBangoPlungerComponent::GetColorForProxy()
{
#if WITH_EDITOR
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return Error;
	}

	ABangoEvent* Event = GetOwner<ABangoEvent>();
	check(Event);

	FLinearColor Color = Event->GetUsesCustomColor() ? Event->GetCustomColor() : ColorBaseMap[Event->GetType()];
	
	const FBangoEventStateFlag& State = Event->GetState();
	
	bool bToggles = Event->IsToggleType();

	double LastHandleDownTime = Event->GetLastActivationTime();
	double LastHandleUpTime = Event->GetLastDeactivationTime();

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
		
		if (!bToggles)
		{
			FLinearColor ActivationColor = BrightenColor(Color);
			FLinearColor DeactivationColor = VeryDarkDesatColor(Color);
			
			double ElapsedTimeSinceLastActivation = GetWorld()->GetTimeSeconds() - LastHandleDownTime;
			double ActivationAlpha = FMath::Clamp(ElapsedTimeSinceLastActivation / RecentPushHandleCooldownTime, 0, 1);
			
			if (IsValid(GWorld) && (ActivationAlpha > 0))
			{
				Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
			}

			double ElapsedTimeSinceLastDeactivation = GetWorld()->GetTimeSeconds() - LastHandleUpTime;
			double DeactivationAlpha = FMath::Clamp(ElapsedTimeSinceLastDeactivation / (2.f * RecentPushHandleCooldownTime), 0, 1);

			if (IsValid(GWorld) && (DeactivationAlpha > 0))
			{
				Color = FMath::Lerp(DeactivationColor, Color, DeactivationAlpha);
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
#else
	return FLinearColor::Black;
#endif
}

bool UBangoPlungerComponent::GetIsPlungerPushed()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()))
	{
		return false;
	}
	
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
			double Elapsed = GetWorld()->GetTimeSeconds() - Event->GetLastActivationTime();

			return Elapsed <= RecentPushHandleCooldownTime;
		}
	}
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
