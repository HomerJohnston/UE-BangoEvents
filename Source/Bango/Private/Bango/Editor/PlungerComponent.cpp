// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Editor/PlungerComponent.h"

#include "Bango/Editor/PlungerSceneProxy.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/CVars.h"
#include "Bango/Log.h"
#include "Bango/Core/BangoEvent.h"

FVector HSVToRGB(double h, double s, double v)
{
	h = h / 360.0;
	
	double r, g, b;

	int i = FMath::Floor(h * 6);
	double f = h * (6.0 - i);
	double p = v * (1.0 - s);
	double q = v * (1.0 - f * s);
	double t = v * (1 - (1 - f) * s);

	switch (i % 6)
	{
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = p; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
		default: r = 1.0, g = 0.0, b = 1.0;
	}

	return FVector(r, g, b);
}

#define BANGO_COLOR_HSV(Name, H, S, L) FLinearColor Name{HSVToRGB(H, S, L)};
#define BANGO_COLOR_RGB(Name, R, G, B) FLinearColor Name{R, G, B};

// Dark color; normal
// Bright color; active
// Light desat color; frozen
// Dark desat color; expired


//				Name				R		G		B
BANGO_COLOR_RGB(BrightRed,			4.00,	0.40,	0.40);
BANGO_COLOR_RGB(LightRedDesat,		0.60,	0.30,	0.30);
BANGO_COLOR_RGB(DarkRedDesat,		0.06,	0.04,	0.04);
BANGO_COLOR_RGB(DarkRed,			0.20,	0.00,	0.00);
	
BANGO_COLOR_RGB(BrightGreen,		0.40,	4.00,	0.40);
BANGO_COLOR_RGB(LightGreenDesat,	0.30,	0.60,	0.30);
BANGO_COLOR_RGB(DarkGreenDesat,		0.04,	0.06,	0.04);
BANGO_COLOR_RGB(DarkGreen,			0.00,	0.20,	0.00);
	
BANGO_COLOR_RGB(BrightBlue,			0.20,	0.20,	5.00);
BANGO_COLOR_RGB(LightBlueDesat,		0.30,	0.30,	0.60);
BANGO_COLOR_RGB(DarkBlueDesat,		0.04,	0.04,	0.06);
BANGO_COLOR_RGB(DarkBlue,			0.00,	0.00,	0.20);

BANGO_COLOR_RGB(BrightOrange,		3.00,	2.50,	0.00);
BANGO_COLOR_RGB(LightOrangeDesat,	0.50,	0.40,	0.30);
BANGO_COLOR_RGB(DarkOrangeDesat,	0.06,	0.05,	0.04);
BANGO_COLOR_RGB(DarkOrange,			0.15,	0.05,	0.00);

BANGO_COLOR_RGB(LightGrey,			0.50,	0.50,	0.50);
BANGO_COLOR_RGB(DarkGrey,			0.02,	0.02,	0.02);
	
BANGO_COLOR_RGB(Error,				1.00,	0.00,	1.00);

TMap<EBangoEventState, FLinearColor> BangColorMap
{
	{ EBangoEventState::NONE,		DarkRed },
	{ EBangoEventState::Active,		BrightRed },
	{ EBangoEventState::Frozen,		LightRedDesat },
	{ EBangoEventState::Expired,	DarkRedDesat }
};

TMap<EBangoEventState, FLinearColor> ToggleColorMap
{
	{ EBangoEventState::NONE,		DarkGreen },
	{ EBangoEventState::Active,		BrightGreen },
	{ EBangoEventState::Frozen,		LightGreenDesat },
	{ EBangoEventState::Expired,	DarkGreenDesat }
};

TMap<EBangoEventState, FLinearColor> InstancedColorMap
{
	{ EBangoEventState::NONE,		DarkBlue },
	{ EBangoEventState::Active,		BrightBlue },
	{ EBangoEventState::Frozen,		LightBlueDesat },
	{ EBangoEventState::Expired,	DarkBlueDesat }
};

TMap<EBangoEventType, TMap<EBangoEventState, FLinearColor>> EventColorMap
{
	{ EBangoEventType::Bang, BangColorMap },
	{ EBangoEventType::Toggle, ToggleColorMap },
	{ EBangoEventType::Instanced, InstancedColorMap }
};

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

	ABangoEvent* Event = GetOwner<ABangoEvent>();
	check(Event);

	const FBangoEventStateFlag& State = Event->GetState();
	bool bToggles = Event->GetToggles();
	double LastHandleDownTime = Event->GetLastStartActionsTime();
	double LastHandleUpTime = Event->GetLastStopActionsTime();

	if (Event->GetType() >= EBangoEventType::MAX)
	{
		return Error;
	}
	
	TMap<EBangoEventState, FLinearColor>& ColorMap = EventColorMap[Event->GetType()]; 
	
	if (World->IsGameWorld())
	{
		if (State.HasFlag(EBangoEventState::Active))
		{
			Color = ColorMap[EBangoEventState::Active];
		}
		else if (State.HasFlag(EBangoEventState::Expired))
		{
			Color = ColorMap[EBangoEventState::Expired];
		}
		else if (State.HasFlag(EBangoEventState::Frozen))
		{
			Color = ColorMap[EBangoEventState::Frozen];
		}
		else if (State.HasFlag(EBangoEventState::Initialized))
		{
			Color = ColorMap[EBangoEventState::NONE];		
		}

		if (!bToggles)
		{
			FLinearColor ActiveColor = ColorMap[EBangoEventState::Active];
			
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
		return ColorMap[EBangoEventState::NONE];
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
