// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoEvent.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/BangoTrigger.h"
#include "Bango/Editor/PlungerComponent.h"
#include "Bango/Core/BangoEventProcessor.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "VisualLogger/VisualLogger.h"

#if WITH_EDITOR
#include "Engine/Canvas.h"
#include "Debug/DebugDrawService.h"
#endif

#if WITH_EDITORONLY_DATA
// TODO FText
TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> ABangoEvent::BangoEventsShowFlag(TEXT("BangoEventsShowFlag"), true, EShowFlagGroup::SFG_Developer, FText(INVTEXT("Bango Events")));
#endif

// ============================================================================================
// Constructor
// ============================================================================================
ABangoEvent::ABangoEvent()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	PlungerComponent = CreateEditorOnlyDefaultSubobject<UBangoPlungerComponent>("Plunger");

	if (PlungerComponent)
	{
		PlungerComponent->SetupAttachment(RootComponent);

		if (!IsRunningCommandlet())
		{
			// TODO copy code from arrow component
		}
	}

	OverrideDisplayMesh = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>("CustomMesh");

	if (OverrideDisplayMesh)
	{
		OverrideDisplayMesh->SetupAttachment(RootComponent);
		OverrideDisplayMesh->SetCastShadow(false);
		OverrideDisplayMesh->SetHiddenInGame(true);
	}
#endif
}

// ------------------------------------------
// Settings Getters
// ------------------------------------------
#if WITH_EDITORONLY_DATA
FText ABangoEvent::GetDisplayName() const
{
	return DisplayName;
}

bool ABangoEvent::GetUsesCustomColor() const
{
	return bUseCustomColor;
}

FLinearColor ABangoEvent::GetCustomColor() const
{
	return CustomColor;
}
#endif

int32 ABangoEvent::GetTriggerLimit() const
{
	return ActivationLimit;
}

void ABangoEvent::SetTriggerLimit(int32 NewTriggerLimit)
{
	ActivationLimit = NewTriggerLimit;
}

int32 ABangoEvent::GetTriggerCount() const
{
	return ActivationCount;
}

bool ABangoEvent::IsBangType() const
{
	return Type == EBangoEventType::Bang;
}

bool ABangoEvent::IsToggleType() const
{
	return Type == EBangoEventType::Toggle;
}

bool ABangoEvent::IsInstancedType() const
{
	return false;
	//return Type == EBangoEventType::Instanced;
}

EBangoEventType ABangoEvent::GetType() const
{
	return Type;
}

EBangoToggleDeactivateCondition ABangoEvent::GetDeactivateCondition() const
{
	return DeactivateCondition;
}


const TArray<UBangoAction*>& ABangoEvent::GetActions() const
{
	return Actions;
}

bool ABangoEvent::GetUsesCustomMesh() const
{
	return bUseCustomMesh;
}

bool ABangoEvent::GetStartsFrozen() const
{
	return bStartsFrozen;
}

// ------------------------------------------
// State Getters
// ------------------------------------------

bool ABangoEvent::GetIsFrozen() const
{
	return bFrozen;
}

bool ABangoEvent::GetIsExpired() const
{
	return bUseActivationLimit && (ActivationCount >= ActivationLimit);
}

double ABangoEvent::GetLastActivationTime() const
{
	return LastActivationTime;
}

double ABangoEvent::GetLastDeactivationTime() const
{
	return LastDeactivationTime;
}

// ============================================================================================
// API
// ============================================================================================

void ABangoEvent::BeginPlay()
{
	switch (GetType())
	{
		case EBangoEventType::Bang:
		{
			EventProcessor = NewObject<UBangoEventProcessor_Bang>(this);
			break;
		}
		case EBangoEventType::Toggle:
		{
			EventProcessor = NewObject<UBangoEventProcessor_Toggle>(this);
			break;
		}
		/*
		case EBangoEventType::Instanced:
		{
			EventProcessor = NewObject<UBangoEventProcessor_Instanced>(this);
			break;
		}
		*/
		default:
		{
			checkNoEntry();
		}
	}

	for (auto it = Triggers.CreateIterator(); it; ++it)
	{
		UBangoTrigger* Trigger = it->Get();
		
		if (!IsValid(Trigger))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid trigger on event: %s"), *this->GetName());
			it.RemoveCurrent();
			continue;
		}
	}

	EnableTriggers();
	
	SetFrozen(bStartsFrozen);

	Super::BeginPlay();

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	OverrideDisplayMesh->SetHiddenInGame(!DevSettings->GetShowEventsInGame());

#if WITH_EDITOR
	UpdateProxyState();

	if (!DebugDrawService_Game.IsValid())
	{
		DebugDrawService_Game = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw));
	}
#endif

#if WITH_EDITORONLY_DATA
	CurrentState.SetFlag(EBangoEventState::Initialized);
#endif
	
	UE_VLOG(this, Bango, Log, TEXT("Event Initialized"));
}

void ABangoEvent::ResetTriggerCount(bool bUnfreeze)
{
	ActivationCount = 0;

	if (bUnfreeze)
	{
		SetFrozen(false);
	}
}

void ABangoEvent::Activate(UObject* ActivationInstigator)
{
	if (GetIsFrozen() || GetIsExpired())
	{
		return;
	}
	
	if (EventProcessor->ActivateFromTrigger(ActivationInstigator))
	{
		ActivationCount++;

		LastActivationTime = GetWorld()->GetTimeSeconds();
		
		OnBangoEventActivated.Broadcast(this, ActivationInstigator);
	}
	
	if (bFreezeWhenExpired && GetIsExpired())
	{
		SetFrozen(true);
	}

#if ENABLE_VISUAL_LOG
	VLOG_Generic("Activate", FColor::Green, ActivationInstigator);
#endif

#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::Deactivate(UObject* DeactivationInstigator)
{
	if (EventProcessor->DeactivateFromTrigger(DeactivationInstigator))
	{
		LastDeactivationTime = GetWorld()->GetTimeSeconds();

		OnBangoEventDeactivated.Broadcast(this, DeactivationInstigator);
	}
	
#if ENABLE_VISUAL_LOG
	VLOG_Generic("Deactivate", FColor::Red, DeactivationInstigator);
#endif
	
#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::SetFrozen(bool bFreeze)
{
	if (bFreeze == bFrozen && HasActorBegunPlay())
	{
		return;
	}
	
	bFrozen = bFreeze;

#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::EnableTriggers()
{
	for (UBangoTrigger* Trigger : Triggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->SetEnabled(true);
		}
	}
}

void ABangoEvent::DisableTriggers()
{
	for (UBangoTrigger* Trigger : Triggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->SetEnabled(false);
		}
	}
}

// Editor ---------------------------------------

#if WITH_EDITOR
const FBangoEventStateFlag& ABangoEvent::GetState() const
{
	return CurrentState;
}
#endif

#if WITH_EDITOR
bool ABangoEvent::HasCurrentState(EBangoEventState State)
{
	return CurrentState.HasFlag(State);
}
#endif

#if WITH_EDITOR
void ABangoEvent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DebugDrawService_Editor.IsValid())
	{
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw));
	}

	if (bUseCustomMesh && IsValid(CustomMesh))
	{
		OverrideDisplayMesh->SetStaticMesh(CustomMesh);
		OverrideDisplayMesh->SetVisibility(true);

		OverrideDisplayMesh->SetWorldScale3D(FVector(CustomMeshScale));
		OverrideDisplayMesh->SetRelativeLocation(FVector(0, 0, CustomMeshBaseOffset + CustomMeshOffset));
	}
	else
	{
		OverrideDisplayMesh->SetStaticMesh(nullptr);
		OverrideDisplayMesh->SetVisibility(false);
	}
	
	UpdateProxyState();
}
#endif

#if WITH_EDITOR
void ABangoEvent::UpdateProxyState()
{
	if (GetWorld()->IsGameWorld())
	{
		CurrentState.SetFlag(EBangoEventState::Active, EventProcessor->GetInstigatorsNum() > 0);
		CurrentState.SetFlag(EBangoEventState::Frozen, GetIsFrozen());
		CurrentState.SetFlag(EBangoEventState::Expired, GetIsExpired());
	}
	
	if (bUseCustomMesh && IsValid(CustomMaterialDynamic))
	{
		FLinearColor C = GetColorForProxy();
		
		CustomMaterialDynamic->SetVectorParameterValue("Color", C);
	}
}
#endif

#if WITH_EDITOR
// TODO see void FEQSRenderingDebugDrawDelegateHelper::DrawDebugLabels(UCanvas* Canvas, APlayerController* PC) they skip drawing if the canvas isn't from the correct world, do I need to do this?
void ABangoEvent::DebugDraw(UCanvas* Canvas, APlayerController* Cont) const
{
	UWorld* World = GetWorld();
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	FVector ScreenLocation;
	double Distance = GetScreenLocation(Canvas, ScreenLocation);
	
	if  (
		   (!ABangoEvent::BangoEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags)) ||
		   (!IsValid(World)) ||
		   (World->IsGameWorld() && !DevSettings->GetShowEventsInGame()) ||
		   (Distance < 0.0)	||
		   (Distance > FMath::Square(GetDefault<UBangoDevSettings>()->GetFarDisplayDistance()))
		)
	{
		return;
	}
	
	FCanvasTextItem HeaderText = GetDebugHeaderText(ScreenLocation);
	Canvas->DrawItem(HeaderText);

	if (Distance < FMath::Square(DevSettings->GetNearDisplayDistance()))
	{
		TDelegate<TArray<FString>()> DataGetter;

		if (GetWorld()->IsGameWorld())
		{
			DataGetter = TDelegate<TArray<FString>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Game);
		}
		else
		{
			DataGetter = TDelegate<TArray<FString>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Editor);
		}
	
		// I could have just use newlines in a single FCanvasTextItem but there's no way to set up text justification nicely in it, text is always left justified.
		// By setting up an array of individual items I can keep them all centre justified and manually offset each one.
		TArray<FCanvasTextItem> DataText = GetDebugDataText(ScreenLocation, DataGetter);

		for (FCanvasTextItem& Text : DataText)
		{
			Canvas->DrawItem(Text);	
		}
	}

	for (UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			continue;
		}
		
		Action->DebugDraw(Canvas, Cont);
	}
}
#endif

#if WITH_EDITOR
double ABangoEvent::GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation) const
{
	// Settings
	double X, Y;
	
	FVector WorldCameraPos;
	FVector WorldCameraDir;

	// Validity Logic
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), WorldCameraPos, WorldCameraDir);

	FVector WorldDrawLocation = GetActorLocation() + FVector(0,0,100);

	double DistSquared = FVector::DistSquared(WorldDrawLocation, WorldCameraPos);

	FVector VectorToWorldDrawLocation = WorldDrawLocation - WorldCameraPos;

	if ((VectorToWorldDrawLocation | WorldCameraDir) < 0.0)
	{
		return -1;
	}

	ScreenLocation = Canvas->Project(WorldDrawLocation);

	return DistSquared;
}
#endif

#if WITH_EDITOR
FCanvasTextItem ABangoEvent::GetDebugHeaderText(const FVector& ScreenLocationCentre) const
{	
	UFont* TextFont = GEngine->GetMediumFont();

	FVector2D HeaderTextPos(ScreenLocationCentre.X, ScreenLocationCentre.Y - 8);

	FText Display;
	
	if (DisplayName.IsEmpty())
	{
		Display = FText::FromString(GetActorLabel());
	}
	else
	{
		Display = DisplayName;
	}

	FColor HeaderColor = (HasInvalidData() ? FColor::Red : FColor::White);
	
	FCanvasTextItem Text(HeaderTextPos, Display, TextFont, HeaderColor);
	Text.bCentreX = true;
	Text.bCentreY = true;
	Text.bOutlined = true;
	Text.Scale = FVector2d(1.2, 1.2);

	return Text;
}
#endif

#if WITH_EDITOR
TArray<FCanvasTextItem> ABangoEvent::GetDebugDataText(const FVector& ScreenLocationCentre, TDelegate<TArray<FString>()> DataGetter) const
{
	UFont* TextFont = GEngine->GetMediumFont();

	FVector2D DataTextPos(ScreenLocationCentre.X, ScreenLocationCentre.Y + 8);

	TArray<FString> Data = DataGetter.Execute();

	TArray<FCanvasTextItem> CanvasTextItems;

	const double LineOffset = 16;
	double CurrentLineOffset = 0;
	
	for(const FString& S : Data)
	{
		FCanvasTextItem Text(DataTextPos, FText::FromString(S), TextFont, FColor::White);
		Text.bCentreX = false;
		Text.Position.Y += CurrentLineOffset;
		Text.Scale = FVector2d(1.1, 1.1);
		CanvasTextItems.Add(Text);

		CurrentLineOffset += LineOffset;
	}
	
	return CanvasTextItems;
}
#endif

#if WITH_EDITOR
TArray<FString> ABangoEvent::GetDebugDataString_Editor() const
{
	TArray<FString> Data;
	
	if (bUseActivationLimit)
	{
		Data.Add(FString::Printf(TEXT("Activation Limit: %i"), ActivationLimit));
	}
	else
	{
		Data.Add(TEXT("Activation Limit: Infinite"));
	}

	if (Triggers.IsEmpty())
	{
		Data.Add("NO TRIGGERS!");
	}
	
	for (UBangoTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{
			Data.Add("NULL TRIGGER");
			continue;
		}
		
		TStringBuilder<128> TriggerEntry;

		TriggerEntry.Append("Trgr: ");
		
		TriggerEntry.Append(Trigger->GetDisplayName().ToString());

		// TODO: Hook to add debug text for triggers
		
		Data.Add(TriggerEntry.ToString());
	}

	if (Actions.IsEmpty())
	{
		Data.Add("NO ACTIONS!");
	}
	
	for (UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			Data.Add("NULL ACTION");
			continue;
		}
		
		TStringBuilder<128> ActionEntry;

		ActionEntry.Append("Actn: ");

		ActionEntry.Append(*Action->GetDisplayName().ToString());

		if (Action->GetUseStartDelay() || Action->GetUseStopDelay())
		{
			ActionEntry.AppendChar(' ');
			
			ActionEntry.AppendChar('(');

			bool bShowSeparator = false;
			
			if (Action->GetUseStartDelay())
			{
				ActionEntry.Append(FString::Printf(TEXT("Start Delay: %.2f"), Action->GetStartDelay()));
				
				bShowSeparator = true;
			}

			if (Action->GetUseStopDelay())
			{
				if (bShowSeparator)
				{
					ActionEntry.Append(" / ");
				}
				
				ActionEntry.Append(FString::Printf(TEXT("Stop Delay: %.2f"), Action->GetStopDelay()));
			}
			
			ActionEntry.AppendChar(')');
		}
		
		Data.Add(ActionEntry.ToString());
	}
	

	return Data;
}
#endif

#if WITH_EDITOR
TArray<FString> ABangoEvent::GetDebugDataString_Game() const
{
	TArray<FString> Data; 

	if (bUseActivationLimit)
	{
		Data.Add(FString::Printf(TEXT("(%i/%i)"), ActivationCount, ActivationLimit));
	}

	if (IsToggleType())
	{
		Data.Add(FString::Printf(TEXT("Instigators: %i"), EventProcessor->GetInstigatorsNum()));
	}
	
	return Data;
}
#endif

#if WITH_EDITOR
bool ABangoEvent::HasInvalidData() const
{
	if (Triggers.IsEmpty())
	{
		return true;
	}

	if (Actions.IsEmpty())
	{
		return true;
	}
	
	for(UBangoTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{
			return true;
		}
	}

	for(UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			return true;
		}
	}

	return false;
}

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


FLinearColor ABangoEvent::GetColorForProxy() const
{
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return Error;
	}

	FLinearColor Color;

	if (GetUsesCustomColor())
	{
		Color = GetCustomColor();
	}
	else
	{
		FLinearColor* MapColor = ColorBaseMap.Find(GetType());
	
		if (!MapColor)
		{
			Color = FColor::Magenta;
		}
		else
		{
			Color = *MapColor;
		}
	}
	
	const FBangoEventStateFlag& State = GetState();
	
	bool bToggles = IsToggleType();

	double LastHandleDownTime = GetLastActivationTime();
	double LastHandleUpTime = GetLastDeactivationTime();

	if (GetType() >= EBangoEventType::MAX)
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
			double ActivationAlpha = FMath::Clamp(ElapsedTimeSinceLastActivation / 0.2, 0, 1);
			
			if (IsValid(GWorld) && (ActivationAlpha > 0))
			{
				Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
			}
			
			double ElapsedTimeSinceLastDeactivation = GetWorld()->GetTimeSeconds() - LastHandleUpTime;
			double DeactivationAlpha = FMath::Clamp(ElapsedTimeSinceLastDeactivation / (2.f * 0.2), 0, 1);
			
			if (IsValid(GWorld) && (DeactivationAlpha > 0))
			{
				Color = FMath::Lerp(DeactivationColor, Color, DeactivationAlpha);
			}
		}
		
		return Color;
	}
	else if (GetWorld()->IsEditorWorld())
	{
		if (GetStartsFrozen())
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

#if ENABLE_VISUAL_LOG
void ABangoEvent::VLOG_Generic(FString Text, FColor Color, UObject* EventInstigator) const
{
	{
		if (AActor* InstigatorActor = Cast<AActor>(EventInstigator))
		{
			UE_VLOG_LOCATION(this, Bango, Log, GetActorLocation(), 50.0, Color, TEXT("%s"), *Text);
			UE_VLOG_SEGMENT(this, Bango, Log, GetActorLocation(), InstigatorActor->GetActorLocation(), Color, TEXT(""));
			UE_VLOG_LOCATION(this, Bango, Log, InstigatorActor->GetActorLocation(), 50.0, Color, TEXT("%s"), *EventInstigator->GetName());
		}
		else
		{
			UE_VLOG_LOCATION(this, Bango, Log, GetActorLocation(), 50.0, Color, TEXT("%s %s"), *Text, *EventInstigator->GetName());
		}
	}
}
#endif