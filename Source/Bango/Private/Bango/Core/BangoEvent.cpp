// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoEvent.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/TriggerCondition.h"
#include "Bango/Editor/PlungerComponent.h"
#include "Editor/EditorEngine.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "VisualLogger/VisualLogger.h"

#if WITH_EDITOR
#include "Editor.h"
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

bool ABangoEvent::GetToggles()
{
	return Type >= EBangoEventType::Toggle;
}

bool ABangoEvent::GetIsInstanced()
{
	return Type >= EBangoEventType::Instanced;
}

EBangoEventType ABangoEvent::GetType()
{
	return Type;
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
	return bUseTriggerLimit && (TriggerCount >= TriggerLimit);
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

	if (!GetToggles() && StopTriggers.Num() > 0)
	{
		// TODO editor setting to throw serialization warnings
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
		DebugDrawService_Game = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw));
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
	
	if (GetToggles())
	{
		int32 NewIndex = Instigators.Add(NewInstigator);

		// Don't run if this is a single-state event and this isn't the first instigator
		if (Type < EBangoEventType::Instanced && NewIndex > 0)
		{
			bRun = false;
		}
	}
	
	if (bRun)
	{
		StartActions(NewInstigator);
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
		return;
	}
	
	bool bIgnore = false;

	if (GetIsInstanced())
	{
		// TODO implement queuing/max concurrent instigators
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
		StopActions(OldInstigator);
	}

	Instigators.RemoveAt(Index);

	// If it's a queued event, and if we just removed the first instigator, run the next instigator 
	if (false && GetIsInstanced() && /* TODO: implement queuing/max conccurent instigators */ Index == 0 && Instigators.Num() > 0)
	{
		//RunStartActions(Instigators[0]);
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

void ABangoEvent::StartActions(UObject* NewInstigator)
{	
	LastStartActionsTime = GetWorld()->GetTimeSeconds();

	// TODO I can't include the event's trigger delay in the delegate execution with my current setup
	OnBangoEventActivated.Broadcast(this, NewInstigator);
	
	double Delay = bUseStartTriggerDelay ? StartTriggerDelay : 0.0;

	// for an instanced event, we treat the assigned action as a template object
	if (GetIsInstanced())
	{
		FBangoEventInstigatorActions* Cur = InstancedActions.Find(NewInstigator);
		
		if (!Cur)
		{
			FBangoEventInstigatorActions NewInstancedActions;

			for (UBangoAction* Template : Actions)
			{
				UBangoAction* ActionInstance = NewObject<UBangoAction>(this, Template->GetClass(), Template->GetFName(), EObjectFlags::RF_NoFlags, Template);
				NewInstancedActions.Actions.Add(ActionInstance);
			}

			Cur = &InstancedActions.Add(NewInstigator, NewInstancedActions);
		}

		for (UBangoAction* Action : Cur->Actions)
		{
			if (!IsValid(Action))
			{
				UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
				continue;
			}
			
			Action->StartInternal(this, NewInstigator, Delay);
		}
	}
	else
	{
		for (UBangoAction* Action : Actions)
		{
			if (!IsValid(Action))
			{
				UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
				continue;
			}
			
			Action->StartInternal(this, NewInstigator, Delay);
		}
	}
}

void ABangoEvent::StopActions(UObject* OldInstigator)
{
	LastStopActionsTime = GetWorld()->GetTimeSeconds();

	// TODO make sure this is proper, do I need to check stuff?
	OnBangoEventActivated.Broadcast(this, OldInstigator);
	
	double Delay = bUseStopTriggerDelay ? StopTriggerDelay : 0.0;

	if (GetIsInstanced())
	{
		FBangoEventInstigatorActions* Cur = InstancedActions.Find(OldInstigator);

		if (!Cur)
		{
			return;
		}

		for (UBangoAction* Action : Cur->Actions)
		{
			if (!IsValid(Action))
			{
				UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
				continue;
			}
			
			Action->StopInternal(Delay);
		}

		InstancedActions.Remove(OldInstigator);

	}
	else
	{
		for (UBangoAction* Action : Actions)
		{
			if (!IsValid(Action))
			{
				UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
				continue;
			}
		
			Action->StopInternal(Delay);
		}
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
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ABangoEvent::DebugDraw));
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
bool ABangoEvent::CanEditChange(const FProperty* Property) const
{
	//if (Property->GetFName() == )
	return Super::CanEditChange(Property);
}
#endif

#if WITH_EDITOR
void ABangoEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

#if WITH_EDITOR
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

	FVector WorldLocation = GetActorLocation() + FVector(0,0,100);

	ScreenLocation = Canvas->Project(WorldLocation);

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

	if (bUseTriggerLimit)
	{
		Data.Add(FString::Printf(TEXT("(%i)"), TriggerLimit));
	}

	return Data;
}

TArray<FString> ABangoEvent::GetDebugDataString_Game()
{
	TArray<FString> Data; 

	if (bUseTriggerLimit)
	{
		Data.Add(FString::Printf(TEXT("(%i/%i)"), TriggerCount, TriggerLimit));
	}

	if (GetToggles())
	{
		Data.Add(FString::Printf(TEXT("Instigators: %i"), Instigators.Num()));
	}
	
	return Data;
}

#endif
