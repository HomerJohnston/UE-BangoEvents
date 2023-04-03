#include "Bango/Core/BangoEvent.h"

#include "Editor.h"
#include "Bango/Log.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/TriggerCondition.h"
#include "Bango/Editor/PlungerComponent.h"
#include "Editor/EditorEngine.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "VisualLogger/VisualLogger.h"

#if WITH_EDITOR
#include "Engine/Canvas.h"
#include "Debug/DebugDrawService.h"
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
			//PlungerComponent->
		}
	}
#endif
}


// ------------------------------------------
// Settings Getters
// ------------------------------------------
#if WITH_EDITORONLY_DATA
FText ABangoEvent::GetDisplayName_Implementation()
{
	return DisplayName;
}
#endif

int32 ABangoEvent::GetTriggerLimit()
{
	return TriggerLimit;
}

int32 ABangoEvent::GetTriggerCount()
{
	return TriggerCount;
}

double ABangoEvent::GetStartTriggerDelay()
{
	return bUseStartTriggerDelay ? StartTriggerDelay : 0.0;
}

double ABangoEvent::GetStopTriggerDelay()
{
	return bUseStopTriggerDelay ? StopTriggerDelay : 0.0;
}

bool ABangoEvent::GetStartsAndStops()
{
	return bStartsAndStops;
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

// ============================================================================================
// API
// ============================================================================================
bool ABangoEvent::GetIsExpired()
{
	return bStartsAndStops && bUseTriggerLimit && (TriggerCount >= TriggerLimit);
}

double ABangoEvent::GetLastStartActionsTime()
{
	return LastStartActionsTime;
}

double ABangoEvent::GetLastStopActionsTime()
{
	return LastStopActionsTime;
}

void ABangoEvent::BeginPlay()
{
	for (UBangoTriggerCondition* Trigger : StartTriggers)
	{
		if (!IsValid(Trigger))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid trigger on event: %s"), *this->GetName());
			continue;
		}

		Trigger->OnTrigger.BindDynamic(this, &ThisClass::ActivateFromTrigger);
	}

	if (!bStartsAndStops && StopTriggers.Num() > 0)
	{
		UE_LOG(Bango, Warning, TEXT("Stop triggers exist on event %s but StartsAndStops is false, removing stop triggers"), *this->GetName());
		StopTriggers.Empty();
	}
	
	for (UBangoTriggerCondition* Trigger : StopTriggers)
	{
		if (!IsValid(Trigger))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid trigger on event: %s"), *this->GetName());
			continue;
		}
	
		Trigger->OnTrigger.BindDynamic(this, &ThisClass::DeactivateFromTrigger);
	}
	
	SetFrozen(bStartsFrozen);

	Super::BeginPlay();

#if WITH_EDITOR
	UpdateProxyState();

	if (!DebugDrawService_Game.IsValid())
	{
		DebugDrawService_Game = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw_Game));
	}
#endif

	CurrentState.SetFlag(EBangoEventState::Initialized);
	UE_VLOG(this, Bango, Log, TEXT("Event Initialized"));
}

void ABangoEvent::ResetTriggerCount(bool bUnfreeze)
{
	TriggerCount = 0;

	if (bUnfreeze)
	{
		SetFrozen(false);
	}
}

// TODO: warnings if lots of instigators? Other faster mechanisms to handle lots of instigators?
void ABangoEvent::ActivateFromTrigger(UObject* NewInstigator)
{
	check(!GetIsFrozen());
	
	if (GetIsExpired())
	{
		return;
	}

	bool bRun = true;
	
	if (bStartsAndStops)
	{
		int32 NewIndex = Instigators.Add(NewInstigator);

		// Don't run if this is a single-state event and this isn't the first instigator
		if (!RunStateSettings.bRunForEveryInstigator && NewIndex > 0)
		{
			bRun = false;
		}

		// Don't run if this is a multi-state event but it's set up to require queued instigators and this isn't the first instigator
		if (RunStateSettings.bRunForEveryInstigator && RunStateSettings.bQueueInstigators && NewIndex > 0)
		{
			bRun = false;
		}
	}
	
	if (bRun)
	{
		RunStartActions(NewInstigator);
	}
	
	TriggerCount++;

	if (bUseTriggerLimit && bFreezeWhenExpired && TriggerCount >= TriggerLimit)
	{
		SetFrozen(true);
	}
	
#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::DeactivateFromTrigger(UObject* OldInstigator)
{
	if (Instigators.Num() > 20)
	{
		UE_LOG(Bango, Warning, TEXT("Warning: several instigators present on event %s, deactivation may be slow"), *this->GetName());
	}
	
	int32 Index = Instigators.Find(OldInstigator);

	if (Index == INDEX_NONE)
	{
		// This can happen if the event has been frozen or become expired but it still has some valid instigators.
		check(bFreezeWhenExpired && Instigators.Num() == 0);
		
		return;
	}
	
	bool bIgnore = false;

	if (RunStateSettings.bRunForEveryInstigator)
	{
		if (RunStateSettings.bQueueInstigators && Index != 0)
		{
			bIgnore = true;
		}
	}
	else
	{
		if (Instigators.Num() > 1)
		{
			bIgnore = true;
		}
	}
	
	if (!bIgnore)
	{
		RunStopActions(OldInstigator);
	}

	Instigators.RemoveAt(Index);

	// If it's a queued event, and if we just removed the first instigator, run the next instigator 
	if (RunStateSettings.bRunForEveryInstigator && RunStateSettings.bQueueInstigators && Index == 0 && Instigators.Num() > 0)
	{
		RunStartActions(Instigators[0]);
	}

	if (GetIsFrozen() && Instigators.Num() == 0)
	{
		DisableTriggers(StopTriggers);
	}
	
#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::SetFrozen(bool bNewFrozen)
{
	if (bNewFrozen == bFrozen && HasActorBegunPlay())
	{
		return;
	}
	
	if (!bNewFrozen)
	{
		EnableTriggers(StartTriggers);
		EnableTriggers(StopTriggers);
	}
	else
	{
		DisableTriggers(StartTriggers);

		if (Instigators.Num() == 0)
		{
			DisableTriggers(StopTriggers); // We will also attempt to disable triggers whenever an instigator is removed
		}
	}
	
	bFrozen = bNewFrozen;
	
#if WITH_EDITOR
	UpdateProxyState();
#endif
}

void ABangoEvent::EnableTriggers(TArray<UBangoTriggerCondition*>& Triggers)
{
	for (UBangoTriggerCondition* Trigger : Triggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Enable();
		}
	}
}

void ABangoEvent::DisableTriggers(TArray<UBangoTriggerCondition*>& Triggers)
{
	for (UBangoTriggerCondition* Trigger : Triggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Disable();
		}
	}
}

#if WITH_EDITOR
bool bRunActionsSafetyGate = false;
#endif

void ABangoEvent::RunStartActions(UObject* NewInstigator)
{	
#if WITH_EDITOR
	bRunActionsSafetyGate = true;
#endif

	LastStartActionsTime = GetWorld()->GetTimeSeconds();
	RunActions(NewInstigator, StartActions, (bUseStartTriggerDelay ? StartTriggerDelay : 0.0));
	
#if WITH_EDITOR
	bRunActionsSafetyGate = false;
#endif
}

void ABangoEvent::RunStopActions(UObject* NewInstigator)
{
#if WITH_EDITOR
	bRunActionsSafetyGate = true;
#endif
	
	LastStopActionsTime = GetWorld()->GetTimeSeconds();
	RunActions(NewInstigator, StopActions, (bUseStopTriggerDelay ? StopTriggerDelay : 0.0));
	
#if WITH_EDITOR
	bRunActionsSafetyGate = false;
#endif
}

void ABangoEvent::RunActions(UObject* NewInstigator, TArray<UBangoAction*>& Actions, double Delay)
{
	check(bRunActionsSafetyGate);

#if ENABLE_VISUAL_LOG
	bool bStart = (&Actions == &StartActions);
	
	FString Text = (bStart) ? "Start by" : "Stop by";
	FColor Color = (bStart) ? FColor::Red : FColor::Silver;

	if (AActor* InstigatorAsActor = Cast<AActor>(NewInstigator))
	{
		UE_VLOG_LOCATION(this, Bango, Log, this->GetActorLocation(), 50.0, Color, TEXT("%s"), *Text);
		UE_VLOG_SEGMENT(this, Bango, Log, this->GetActorLocation(), InstigatorAsActor->GetActorLocation(), Color, TEXT(""));
		UE_VLOG_LOCATION(this, Bango, Log, InstigatorAsActor->GetActorLocation(), 50.0, Color, TEXT("%s"), *NewInstigator->GetName());
	}
	else
	{
		UE_VLOG_LOCATION(this, Bango, Log, this->GetActorLocation(), 50.0, Color, TEXT("%s %s"), *Text, *NewInstigator->GetName());
	}
#endif
	
	for (UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s"), *this->GetName());
			continue;
		}
		
		Action->RunInternal(this, NewInstigator, Delay);
	}
}

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
#endif

#if WITH_EDITOR
void ABangoEvent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DebugDrawService_Editor.IsValid())
	{
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw_Editor));
	}	

	if (GetWorld()->IsGameWorld())
	{
		UpdateProxyState();
	}
}
#endif

#if WITH_EDITOR
void ABangoEvent::UpdateProxyState()
{
	CurrentState.SetFlag(EBangoEventState::Active, Instigators.Num() > 0);
	CurrentState.SetFlag(EBangoEventState::Frozen, GetIsFrozen());
	CurrentState.SetFlag(EBangoEventState::Expired, GetIsExpired());
}
#endif

#if WITH_EDITOR
void ABangoEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

#if WITH_EDITOR
void ABangoEvent::DebugDraw_Editor(UCanvas* Canvas, APlayerController* Cont)
{
	// TODO see void FEQSRenderingDebugDrawDelegateHelper::DrawDebugLabels(UCanvas* Canvas, APlayerController* PC) they skip drawing if the canvas isn't from the correct world, do I need to do this?
	
	UFont* HeaderFont = GEngine->GetLargeFont();
	UFont* TextFont = GEngine->GetTinyFont();
	
	FString HeaderString = "Test 1";
	FString TextString = "Test 2";
	
	FVector WorldLocation = GetActorLocation() + FVector(0,0,100);
	FVector ScreenLocation;

	ScreenLocation = Canvas->Project(WorldLocation);

	FVector2D HeaderTextPos(ScreenLocation.X, ScreenLocation.Y - 8);
	
	FCanvasTextItem HeaderText(HeaderTextPos, DisplayName, HeaderFont, FColor::White);
	HeaderText.bCentreX = true;
	HeaderText.bCentreY = true;
	
	Canvas->DrawItem(HeaderText);
}

void ABangoEvent::DebugDraw_Game(UCanvas* Canvas, APlayerController* Cont)
{
	UFont* HeaderFont = GEngine->GetLargeFont();
	UFont* TextFont = GEngine->GetTinyFont();
	
	FString HeaderString = "Test 1";
	FString TextString = "Test 2";
	
	FVector WorldLocation = GetActorLocation() + FVector(0,0,100);
	FVector ScreenLocation;

	ScreenLocation = Canvas->Project(WorldLocation);

	FVector2D HeaderTextPos(ScreenLocation.X, ScreenLocation.Y - 8);
	
	FCanvasTextItem HeaderText(HeaderTextPos, DisplayName, HeaderFont, FColor::White);
	HeaderText.bCentreX = true;
	HeaderText.bCentreY = true;
	
	Canvas->DrawItem(HeaderText);

	int32 CurrentOffset = 8;

	if (bUseTriggerLimit)
	{
		FVector2D SubTextPos(ScreenLocation.X, ScreenLocation.Y + CurrentOffset);

		FString S = FString::Printf(TEXT("(%i / %i)"), TriggerCount, TriggerLimit);
		FCanvasTextItem InfoText(SubTextPos, FText::FromString(S), TextFont, FColor::White);
		InfoText.bCentreX = true;
		InfoText.bCentreY = true;

		Canvas->DrawItem(InfoText);

		CurrentOffset += 8;
	}
}
#endif
