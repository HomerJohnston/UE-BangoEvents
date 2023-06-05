﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoEvent.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/BangoTrigger.h"
#include "Bango/Editor/PlungerComponent.h"
//#include "Editor/EditorEngine.h"
#include "Bango/Core/BangoEventProcessor.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "VisualLogger/VisualLogger.h"

#if WITH_EDITOR
#include "Bango/CVars.h"
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
			//PlungerComponent->
		}
	}
#endif
}


// ------------------------------------------
// Settings Getters
// ------------------------------------------
#if WITH_EDITORONLY_DATA
FText ABangoEvent::GetDisplayName()
{
	return DisplayName;
}

bool ABangoEvent::GetUsesCustomColor()
{
	return bUseCustomColor;
}

FLinearColor ABangoEvent::GetCustomColor()
{
	return CustomColor;
}
#endif

int32 ABangoEvent::GetTriggerLimit()
{
	return ActivationLimit;
}

void ABangoEvent::SetTriggerLimit(int32 NewTriggerLimit)
{
	ActivationLimit = NewTriggerLimit;
}

int32 ABangoEvent::GetTriggerCount()
{
	return ActivationCount;
}

bool ABangoEvent::IsBangType()
{
	return Type == EBangoEventType::Bang;
}

bool ABangoEvent::IsToggleType()
{
	return Type == EBangoEventType::Toggle;
}

bool ABangoEvent::IsInstancedType()
{
	return false;
	//return Type == EBangoEventType::Instanced;
}

EBangoEventType ABangoEvent::GetType()
{
	return Type;
}

EBangoToggleDeactivateCondition ABangoEvent::GetDeactivateCondition()
{
	return DeactivateCondition;
}


const TArray<UBangoAction*>& ABangoEvent::GetActions()
{
	return Actions;
}

bool ABangoEvent::GetStartsFrozen()
{
	return bStartsFrozen;
}

// ------------------------------------------
// State Getters
// ------------------------------------------

bool ABangoEvent::GetIsFrozen()
{
	return bFrozen;
}

bool ABangoEvent::GetIsExpired()
{
	return bUseActivationLimit && (ActivationCount >= ActivationLimit);
}

double ABangoEvent::GetLastActivationTime()
{
	return LastActivationTime;
}

double ABangoEvent::GetLastDeactivationTime()
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

	SetFrozen(bStartsFrozen);

	Super::BeginPlay();

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

void ABangoEvent::Activate(UObject* NewInstigator)
{
	if (GetIsFrozen() || GetIsExpired())
	{
		return;
	}
	
	if (EventProcessor->ActivateFromTrigger(NewInstigator))
	{
		ActivationCount++;

		LastActivationTime = GetWorld()->GetTimeSeconds();
		
		OnBangoEventActivated.Broadcast(this, NewInstigator);
	}
	
	if (bFreezeWhenExpired && GetIsExpired())
	{
		SetFrozen(true);
	}
	
#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::Deactivate(UObject* OldInstigator)
{
	if (EventProcessor->DeactivateFromTrigger(OldInstigator))
	{
		LastDeactivationTime = GetWorld()->GetTimeSeconds();

		OnBangoEventDeactivated.Broadcast(this, OldInstigator);
	}
	
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
	
	if (bFreeze)
	{
		DisableTriggers();

		// Freezing is intended to halt new starts. It is not intended to prevent stopping running actions.
		// We will also attempt to disable triggers whenever an instigator is removed.
	}
	else
	{
		EnableTriggers();
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

#if ENABLE_VISUAL_LOG
void DoVLOG(AActor* Target, FString Text, FColor Color, UObject* NewInstigator)
{
	if (AActor* InstigatorAsActor = Cast<AActor>(NewInstigator))
	{
		UE_VLOG_LOCATION(Target, Bango, Log, Target->GetActorLocation(), 50.0, Color, TEXT("%s"), *Text);
		UE_VLOG_SEGMENT(Target, Bango, Log, Target->GetActorLocation(), InstigatorAsActor->GetActorLocation(), Color, TEXT(""));
		UE_VLOG_LOCATION(Target, Bango, Log, InstigatorAsActor->GetActorLocation(), 50.0, Color, TEXT("%s"), *NewInstigator->GetName());
	}
	else
	{
		UE_VLOG_LOCATION(Target, Bango, Log, Target->GetActorLocation(), 50.0, Color, TEXT("%s %s"), *Text, *NewInstigator->GetName());
	}
}
#endif

// Editor ---------------------------------------

#if WITH_EDITOR
const FBangoEventStateFlag& ABangoEvent::GetState() const
{
	return CurrentState;
}

bool ABangoEvent::HasCurrentState(EBangoEventState State)
{
	return CurrentState.HasFlag(State);
}

void ABangoEvent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DebugDrawService_Editor.IsValid())
	{
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw));
	}	

	if (GetWorld()->IsGameWorld())
	{
		UpdateProxyState();
	}
}

void ABangoEvent::UpdateProxyState()
{
	CurrentState.SetFlag(EBangoEventState::Active, EventProcessor->GetInstigatorsNum() > 0);
	CurrentState.SetFlag(EBangoEventState::Frozen, GetIsFrozen());
	CurrentState.SetFlag(EBangoEventState::Expired, GetIsExpired());
}

// TODO see void FEQSRenderingDebugDrawDelegateHelper::DrawDebugLabels(UCanvas* Canvas, APlayerController* PC) they skip drawing if the canvas isn't from the correct world, do I need to do this?
void ABangoEvent::DebugDraw(UCanvas* Canvas, APlayerController* Cont)
{
	if (!ABangoEvent::BangoEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags))
	{
		return;
	}
	
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return;
	}
	
	if (World->IsGameWorld() && !GhostPepperGames::Bango::bShowEventsInGame)
	{
		return;
	}

	FVector ScreenLocation;
	
	if (!GetScreenLocation(Canvas, ScreenLocation))
	{
		return;
	}
	
	FCanvasTextItem HeaderText = GetDebugHeaderText(ScreenLocation);
	Canvas->DrawItem(HeaderText);

	TDelegate<TArray<FString>()> DataGetter;

	if (World->IsGameWorld())
	{
		DataGetter = TDelegate<TArray<FString>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Game);
	}
	else
	{
		DataGetter = TDelegate<TArray<FString>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Editor);
	}

	// I could have just use newlines in a single FCanvasTextItem but there's no way to set up text justification nicely in it, text is always left justified.
	TArray<FCanvasTextItem> DataText = GetDebugDataText(ScreenLocation, DataGetter);

	for (FCanvasTextItem& Text : DataText)
	{
		Canvas->DrawItem(Text);	
	}
}

bool ABangoEvent::GetScreenLocation(UCanvas* Canvas, FVector& ScreenLocation)
{	
	// Settings
	double X, Y;
	
	FVector WorldCameraPos;
	FVector WorldCameraDir;

	const double ThresholdDistance = GetWorld()->IsGameWorld() ? 2500 : 5000; // TODO editor settings for display distances
	const double Threshold = FMath::Square(ThresholdDistance);

	// Validity Logic
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), WorldCameraPos, WorldCameraDir);

	FVector WorldDrawLocation = GetActorLocation() + FVector(0,0,100);
	
	if (FVector::DistSquared(WorldDrawLocation, WorldCameraPos) > Threshold)
	{
		return false;
	}

	FVector VectorToWorldDrawLocation = WorldDrawLocation - WorldCameraPos;

	if ((VectorToWorldDrawLocation | WorldCameraDir) < 0.0)
	{
		return false;
	}

	ScreenLocation = Canvas->Project(WorldDrawLocation);

	return true;
}

FCanvasTextItem ABangoEvent::GetDebugHeaderText(const FVector& ScreenLocationCentre)
{	
	UFont* TextFont = GEngine->GetMediumFont();
	
	FVector2D HeaderTextPos(ScreenLocationCentre.X, ScreenLocationCentre.Y - 8);
	
	FCanvasTextItem Text(HeaderTextPos, DisplayName, TextFont, FColor::White);
	Text.bCentreX = true;
	Text.bCentreY = true;
	Text.bOutlined = true;

	return Text;
}

TArray<FCanvasTextItem> ABangoEvent::GetDebugDataText(const FVector& ScreenLocationCentre, TDelegate<TArray<FString>()> DataGetter)
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
		Text.bCentreX = true;
		Text.Position.Y += CurrentLineOffset;
		CanvasTextItems.Add(Text);

		CurrentLineOffset += LineOffset;
	}
	
	return CanvasTextItems;
}

TArray<FString> ABangoEvent::GetDebugDataString_Editor()
{
	TArray<FString> Data;
	
	if (bUseActivationLimit)
	{
		Data.Add(FString::Printf(TEXT("(Activation Limit: %i)"), ActivationLimit));
	}

	for (UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			continue;
		}
		
		Data.Add(FString::Printf(TEXT("%s"), *Action->GetDisplayName()));
	}
	

	return Data;
}

TArray<FString> ABangoEvent::GetDebugDataString_Game()
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
