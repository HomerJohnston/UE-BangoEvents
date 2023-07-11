// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Event/BangoEvent.h"

#include "Bango/Utility/Log.h"
#include "Bango/Action/BangoAction.h"
#include "Bango/Trigger/BangoTrigger.h"
#include "Bango/Editor/PlungerComponent.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Utility/BangoColor.h"
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
		OverrideDisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	DebugSignal = EBangoSignal::Activate;
#endif
}

// ------------------------------------------
// Settings Getters and Setters
// ------------------------------------------

#if WITH_EDITORONLY_DATA
FText ABangoEvent::GetDisplayName() const
{
	if (bUseDisplayName)
	{
		return DisplayName;
	}
	
	return FText::FromString(GetActorNameOrLabel());
}

bool ABangoEvent::GetUsesCustomColor() const
{
	return bUseCustomColor;
}

FLinearColor ABangoEvent::GetCustomColor() const
{
	return CustomColor;
}

bool ABangoEvent::GetUsesCustomMesh() const
{
	return bUseCustomMesh;
}
#endif

bool ABangoEvent::GetStartsFrozen() const
{
	return bStartsFrozen;
}

int32 ABangoEvent::GetTriggerLimit() const
{
	return bUseTriggerLimit ? TriggerLimit : -1;
}

void ABangoEvent::SetTriggerLimit(int32 NewTriggerLimit)
{
	TriggerLimit = NewTriggerLimit;
}

int32 ABangoEvent::GetTriggerCount(EBangoSignal Signal) const
{
	const int32* CountPtr = TriggerCounts.Find(Signal);
	return CountPtr ? *CountPtr : 0;
}

// ------------------------------------------
// State Getters
// ------------------------------------------

bool ABangoEvent::GetIsFrozen() const
{
	return bFrozen;
}

bool ABangoEvent::TriggerLimitReached(EBangoSignal Signal) const
{
	if (!bUseTriggerLimit) { return false; }
	
	const int32* CountPtr = TriggerCounts.Find(Signal);
	if (!CountPtr) { return false; }

	return *CountPtr >= TriggerLimit;
}

double ABangoEvent::GetLastTriggerTime(EBangoSignal Signal) const
{
	const double* LastTriggerTime = LastTriggerTimes.Find(Signal);

	if (LastTriggerTime)
	{
		return *LastTriggerTime;
	}

	return INDEX_NONE;
}

// ============================================================================================
// API
// ============================================================================================

void ABangoEvent::BeginPlay()
{
	Super::BeginPlay();

#if !UE_BUILD_SHIPPING
	CleanupInvalidTriggers();
	CleanupInvalidActions();
#endif
	
	if (bStartsFrozen)
	{
		SetFrozen(true);
	}
	else
	{
		SetTriggers(true);
	}

	// TODO should logic like this only be in ResetEvent somehow? Should I call ResetEvent here?
	ResetRemainingTriggerLimits();

#if WITH_EDITOR	
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();

	OverrideDisplayMesh->SetHiddenInGame(!DevSettings->GetShowEventsInGame());

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

#if WITH_EDITOR
	FAutoConsoleVariableSink CVarSink(FConsoleCommandDelegate::CreateUObject(this, &ThisClass::OnCvarChange));
#endif
}

void ABangoEvent::ResetEvent(bool bUnfreeze)
{
	InstigatorData.Empty();
	SetFrozen(!bUnfreeze);

	ResetRemainingTriggerLimits();

#if !UE_BUILD_SHIPPING
	VLOG_Generic("Reset Event", FColor::White, nullptr);
#endif
}

void ABangoEvent::Trigger(EBangoSignal Signal, UObject* NewInstigator)
{
	if (GetIsFrozen())
	{
		return;
	}
	
	if (bUseTriggerLimit && TriggerLimitReached(Signal))
	{
		return;
	}
		
	if (ProcessTriggerSignal(Signal, NewInstigator))
	{
		int32& TriggerCount = TriggerCounts.FindOrAdd(Signal);
		TriggerCount++;

		double& TriggerTime = LastTriggerTimes.FindOrAdd(Signal);
		TriggerTime = GetWorld()->GetTimeSeconds();
		
		if (bUseTriggerLimit && TriggerLimitReached(Signal))
		{
			OnSignalLimitReached.Broadcast(this, Signal);

			RemainingTriggerLimits.Remove(Signal);

			if (bFreezeWhenAllTriggerLimitsReached && RemainingTriggerLimits.IsEmpty())
			{
				SetFrozen(true);
			}
		}				
		
		UpdateProxyState();
		
		OnEventTriggered.Broadcast(this, Signal, NewInstigator);
	}
}

bool ABangoEvent::ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	return false;
}

void ABangoEvent::SetFrozen(bool bNewFrozen)
{
	if (!HasActorBegunPlay()) { return; }
	
	if (bFrozen == bNewFrozen) { return; }
	
	bFrozen = bNewFrozen;

	SetTriggers(!bNewFrozen);
	
#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::ResetRemainingTriggerLimits()
{
	checkNoEntry();
}

void ABangoEvent::CleanupInvalidTriggers()
{
	for (int32 i = Triggers.Num() - 1; i >= 0; --i)
	{
		UBangoTrigger* Trigger = Triggers[i];
		if (!IsValid(Trigger))
		{
			Triggers.RemoveAt(i, 1, false);
			UE_LOG(Bango, Error, TEXT("A null trigger entry was found, this must be fixed! Event: %s"), *GetDisplayName().ToString());
		}
	}

	Triggers.Shrink();
}

void ABangoEvent::CleanupInvalidActions()
{
	for (int32 i = Actions.Num() - 1; i >= 0; --i)
	{
		UBangoAction* Action = Actions[i];
		if (!IsValid(Action))
		{
			Actions.RemoveAt(i, 1, false);
			UE_LOG(Bango, Error, TEXT("A null action entry was found, this must be fixed! Event: %s"), *GetDisplayName().ToString());
		}
	}

	Actions.Shrink();
}

void ABangoEvent::SetTriggers(bool bEnabled)
{
	for (UBangoTrigger* Trigger : Triggers)
	{
		if (bEnabled)
		{
			Trigger->TriggerSignal.AddDynamic(this, &ABangoEvent::Trigger);
		}
		else
		{
			Trigger->TriggerSignal.RemoveDynamic(this, &ABangoEvent::Trigger);
		}
	
		Trigger->SetEnabled(bEnabled);
	}
}

// ================================================================================================
// EDITOR
// ================================================================================================

#if WITH_EDITOR
FLinearColor ABangoEvent::GetColorBase() const
{
	return FColor::Magenta;
}
#endif

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
void ABangoEvent::Destroyed()
{
	UDebugDrawService::Unregister(DebugDrawService_Editor);
	UDebugDrawService::Unregister(DebugDrawService_Game);

	DebugDrawService_Editor.Reset();
	DebugDrawService_Game.Reset();
	
	Super::Destroyed();
}
#endif

#if WITH_EDITOR
void ABangoEvent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bUseCustomMesh && IsValid(CustomMesh))
	{
		OverrideDisplayMesh->SetStaticMesh(CustomMesh);
		OverrideDisplayMesh->SetVisibility(true);

		OverrideDisplayMesh->SetWorldScale3D(FVector(CustomMeshScale));
		OverrideDisplayMesh->SetRelativeLocation(FVector(0, 0, CustomMeshOffsetBase + CustomMeshOffset));
	}
	else
	{
		OverrideDisplayMesh->SetStaticMesh(nullptr);
		OverrideDisplayMesh->SetVisibility(false);
	}
	
	if (!DebugDrawService_Editor.IsValid())
	{
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw));
	}	
	
	UpdateProxyState();
}
#endif

#if WITH_EDITOR
void ABangoEvent::UpdateProxyState()
{	
	if (bUseCustomMesh && IsValid(CustomMaterialDynamic))
	{
		FLinearColor C = GetColorForProxy();
		CustomMaterialDynamic->SetVectorParameterValue("Color", C);
	}

	CurrentState.SetFlag(EBangoEventState::Frozen, GetIsFrozen());
}
#endif

#if WITH_EDITOR
// TODO see void FEQSRenderingDebugDrawDelegateHelper::DrawDebugLabels(UCanvas* Canvas, APlayerController* PC) they skip drawing if the canvas isn't from the correct world, do I need to do this?
void ABangoEvent::DebugDraw(UCanvas* Canvas, APlayerController* PlayerController) const
{
	UWorld* World = GetWorld();

	if (!IsValid(World)) { return; }
	if (!ABangoEvent::BangoEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags)) { return; }

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	
	if (World->IsGameWorld() && !DevSettings->GetShowEventsInGame()) { return; }
	
	double DistanceSqrd;
	FVector ScreenLocation;

	bool bDraw = GetScreenLocation(Canvas, ScreenLocation, DistanceSqrd);
	
	if (!bDraw) { return; }
	if (DistanceSqrd > FMath::Square(GetDefault<UBangoDevSettings>()->GetFarDisplayDistance())) { return; }
	
	double Distance = FMath::Sqrt(DistanceSqrd);
	
	FCanvasTextItem HeaderText = GetDebugHeaderText(ScreenLocation, Distance);
	Canvas->DrawItem(HeaderText);

	if (Distance < DevSettings->GetNearDisplayDistance())
	{
		TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter;

		if (GetWorld()->IsGameWorld())
		{
			DataGetter = TDelegate<TArray<FBangoDebugTextEntry>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Game);
		}
		else
		{
			DataGetter = TDelegate<TArray<FBangoDebugTextEntry>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Editor);
		}
	
		// I could have just use newlines in a single FCanvasTextItem but there's no way to set up text justification nicely in it, text is always left justified.
		// By setting up an array of individual items I can keep them all centre justified and manually offset each one.
		TArray<FCanvasTextItem> DataText = GetDebugDataText(Canvas, ScreenLocation, DataGetter, Distance);

		for (FCanvasTextItem& Text : DataText)
		{
			Canvas->DrawItem(Text);	
		}
	}

	for (UBangoTrigger* Trigger : Triggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->DebugDraw(Canvas, PlayerController);
		}
	}
	
	for (UBangoAction* Action : Actions)
	{
		if (IsValid(Action))
		{
			Action->DebugDraw(Canvas, PlayerController);
		}
	}
}
#endif

#if WITH_EDITOR
bool ABangoEvent::GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation, double& DistSqrd) const
{
	// Settings
	double X, Y;
	
	FVector WorldCameraPos;
	FVector WorldCameraDir;

	// Validity Logic
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), WorldCameraPos, WorldCameraDir);

	FVector WorldDrawLocation = GetActorLocation() + FVector(0,0,100);

	DistSqrd = FVector::DistSquared(WorldDrawLocation, WorldCameraPos);

	FVector VectorToWorldDrawLocation = WorldDrawLocation - WorldCameraPos;

	if ((VectorToWorldDrawLocation | WorldCameraDir) < 0.0)
	{
		return false;
	}

	ScreenLocation = Canvas->Project(WorldDrawLocation);

	return true;
}
#endif

#if WITH_EDITOR
FCanvasTextItem ABangoEvent::GetDebugHeaderText(const FVector& ScreenLocationCentre, double Distance) const
{	
	UFont* TextFont = GEngine->GetLargeFont();
	
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

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	
	float FarDisplayDistance = DevSettings->GetFarDisplayDistance();

	float LerpAlpha = FMath::Clamp((FarDisplayDistance - Distance) / (1.25 * FarDisplayDistance - FarDisplayDistance), 0, 1);
	float ColorAlpha = FMath::Lerp(0.0, 1.0, LerpAlpha);
	float OutlineAlpha = FMath::Lerp(0.0, 1.0, FMath::Square(LerpAlpha));

	FLinearColor HeaderBaseColor = HasInvalidData() ? BangoColor::Orange : FLinearColor::White;
	FLinearColor HeaderColor = HeaderBaseColor;

	if (bUseDisplayName)
	{
		HeaderColor = BangoColor::LightBlue * HeaderBaseColor;
	}

	HeaderColor.A = ColorAlpha;
	
	FCanvasTextItem Text(HeaderTextPos, Display, TextFont, HeaderColor);
	Text.bCentreX = true;
	Text.bCentreY = true;
	Text.bOutlined = true;
	Text.OutlineColor = FLinearColor(0, 0, 0, OutlineAlpha);
	Text.Scale = FVector2d(1.0, 1.0);

	return Text;
}
#endif

#if WITH_EDITOR
TArray<FCanvasTextItem> ABangoEvent::GetDebugDataText(UCanvas* Canvas, const FVector& ScreenLocationCentre, TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter, double Distance) const
{
	UFont* TextFont = GEngine->GetMediumFont();

	FVector2D DataTextPos(ScreenLocationCentre.X, ScreenLocationCentre.Y + 8);

	TArray<FBangoDebugTextEntry> Data = DataGetter.Execute();

	TArray<FCanvasTextItem> CanvasTextItems;

	const double LineOffset = 16;
	double CurrentLineOffset = 0;
	
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();

	float NearDisplayDistance = DevSettings->GetNearDisplayDistance();
	
	float LerpAlpha = FMath::Clamp((NearDisplayDistance - Distance) / (1.25 * NearDisplayDistance - NearDisplayDistance), 0, 1);
	float ColorAlpha = FMath::Lerp(0.0, 1.0, LerpAlpha);
	float OutlineAlpha = FMath::Lerp(0.0, 1.0, FMath::Square(LerpAlpha));
	
	for(const FBangoDebugTextEntry& S : Data)
	{
		float LX = 0;
		float LY = 0;
		float RX = 0;
		float RY = 0;
		
		Canvas->StrLen(TextFont, S.TextL, LX, LY, false);
		Canvas->StrLen(TextFont, S.TextR, RX, RY, false);

		FLinearColor LColor = FLinearColor::White;
		LColor.A = ColorAlpha;
		
		FCanvasTextItem TextLeft(DataTextPos, FText::FromString(S.TextL), TextFont, LColor);
		TextLeft.Position.X -= LX;
		TextLeft.Position.Y += CurrentLineOffset;
		TextLeft.Scale = FVector2d(1.0);
		TextLeft.bOutlined = true;
		TextLeft.OutlineColor = FColor(50, 50, 50, OutlineAlpha);

		FLinearColor RColor = S.Color;
		RColor.A = ColorAlpha;
		
		FCanvasTextItem TextRight(DataTextPos, FText::FromString(S.TextR), TextFont, RColor);
		TextRight.Position.X += 3;
		TextRight.Position.Y += CurrentLineOffset;
		TextRight.Scale = FVector2d(1.0);
		TextRight.bOutlined = true;
		TextRight.OutlineColor = FColor(50, 50, 50, OutlineAlpha);
		
		CanvasTextItems.Add(TextLeft);
		CanvasTextItems.Add(TextRight);

		CurrentLineOffset += LineOffset;
	}
	
	return CanvasTextItems;
}
#endif

#if WITH_EDITOR
TArray<FBangoDebugTextEntry> ABangoEvent::GetDebugDataString_Editor() const
{
	TArray<FBangoDebugTextEntry> Data;
	
	if (bUseTriggerLimit)
	{
		Data.Add(FBangoDebugTextEntry("Activation Limit:", FString::Printf(TEXT("%i"), TriggerLimit)));
	}

	if (Triggers.IsEmpty())
	{
		Data.Add(FBangoDebugTextEntry("Triggers:", "NONE", BangoColor::Orange));
	}
	
	for (UBangoTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{			
			Data.Add(FBangoDebugTextEntry("Trigger:", "NULL TRIGGER", BangoColor::Orange));
		}
		else
		{
			FString Prefix = "Trigger:";
			
			TStringBuilder<128> TriggerEntry;

			TriggerEntry.Append(Trigger->GetDisplayName().ToString());

			Data.Add(FBangoDebugTextEntry(Prefix, TriggerEntry.ToString()));

			Trigger->AppendDebugData(Data);
		}
	}

	if (Actions.IsEmpty())
	{
		Data.Add(FBangoDebugTextEntry("Action:", "NONE", BangoColor::Orange));
	}
	
	for (UBangoAction* Action : Actions)
	{
		TStringBuilder<128> ActionEntry;

		if (!IsValid(Action))
		{
			ActionEntry.Append("NULL");
		}
		else
		{
			ActionEntry.Append(*Action->GetDisplayName().ToString());
		}
		
		Data.Add(FBangoDebugTextEntry("Action:", ActionEntry.ToString()));

		Action->GetDebugDataString(Data);
	}	
	
	return Data;
}
#endif

#if WITH_EDITOR
TArray<FBangoDebugTextEntry> ABangoEvent::GetDebugDataString_Game() const
{
	TArray<FBangoDebugTextEntry> Data; 

	if (bUseTriggerLimit)
	{
		Data.Add(FBangoDebugTextEntry("Activations:", FString::Printf(TEXT("(%i/%i)"), GetTriggerCount(EBangoSignal::Activate), TriggerLimit)));
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

	for (UBangoTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{
			return true;
		}	
	}

	if (Actions.IsEmpty())
	{
		return true;
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

FLinearColor ABangoEvent::GetColorForProxy() const
{
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return BangoColor::Error;
	}
	
	FLinearColor Color = GetUsesCustomColor() ? GetCustomColor() : GetColorBase();
	
	if (World->IsGameWorld())
	{
		const FBangoEventStateFlag& State = GetState();
		
		if (State.HasFlag(EBangoEventState::Active))
		{
			Color = BangoColorOps::BrightenColor(Color);
		}
		else if (State.HasFlag(EBangoEventState::Expired))
		{
			Color = BangoColorOps::DarkDesatColor(Color);
		}
		else if (State.HasFlag(EBangoEventState::Frozen))
		{
			Color = BangoColorOps::LightDesatColor(Color);
		}
		
		return Color;
	}
	else if (GetWorld()->IsEditorWorld())
	{
		return GetStartsFrozen() ? BangoColorOps::LightDesatColor(Color) : Color;
	}
	else
	{
		return BangoColor::Error;
	}
}

void ABangoEvent::OnCvarChange()
{
	if (!bUseCustomMesh || !IsValid(OverrideDisplayMesh))
	{
		return;
	}

	UWorld* World = GetWorld();
	
	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	const IConsoleVariable* ShowInGameCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));
	bool bNewHiddenInGame = !ShowInGameCVar->GetBool();
	
	if (bUseCustomMesh && IsValid(OverrideDisplayMesh))
	{
		OverrideDisplayMesh->SetHiddenInGame(bNewHiddenInGame);
	}
}

void ABangoEvent::TriggerDebugSignal()
{
	ProcessTriggerSignal(DebugSignal, IsValid(DebugSignalInstigator) ? DebugSignalInstigator : this);
}
#endif

#if ENABLE_VISUAL_LOG
void ABangoEvent::VLOG_Generic(FString Text, FColor Color, UObject* LogInstigator) const
{
	AActor* InstigatorActor = Cast<AActor>(LogInstigator);
	
	if (IsValid(InstigatorActor))
	{
		UE_VLOG_LOCATION(this, Bango, Log, GetActorLocation(), 50.0, Color, TEXT("%s"), *Text);
		UE_VLOG_SEGMENT(this, Bango, Log, GetActorLocation(), InstigatorActor->GetActorLocation(), Color, TEXT(""));
		UE_VLOG_LOCATION(this, Bango, Log, InstigatorActor->GetActorLocation(), 50.0, Color, TEXT("%s"), *LogInstigator->GetName());
	}
	else
	{
		UE_VLOG_UELOG(this, Bango, Log, TEXT("%s %s"), *Text, *LogInstigator->GetName());
		//UE_VLOG_LOCATION(this, Bango, Log, GetActorLocation(), 50.0, Color, TEXT("%s %s"), *Text, *EventInstigator->GetName());
	}
}
#endif