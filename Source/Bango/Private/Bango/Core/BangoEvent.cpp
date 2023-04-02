#include "Bango/Core/BangoEvent.h"

#include "Editor.h"
#include "Bango/Log.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/TriggerCondition.h"
#include "Bango/Editor/PlungerComponent.h"
#include "Bango/Subsystems/BangoEngineSubsystem.h"
#include "Components/ShapeComponent.h"
#include "Editor/EditorEngine.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "VisualLogger/VisualLogger.h"

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

#if WITH_EDITOR
	DebugUpdate();
#endif
	
	Super::BeginPlay();

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

	int32 NewIndex = Instigators.Add(NewInstigator);

	bool bRun = true;
	
	if (bStartsAndStops)
	{
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
#if ENABLE_VISUAL_LOG	
	if (AActor* InstigatorAsActor = Cast<AActor>(NewInstigator))
	{
		UE_VLOG_LOCATION(this, Bango, Log, this->GetActorLocation(), 50.0, FColor::Green, TEXT("Start by"));
		UE_VLOG_SEGMENT(this, Bango, Log, this->GetActorLocation(), InstigatorAsActor->GetActorLocation(), FColor::Green, TEXT(""));
		UE_VLOG_LOCATION(this, Bango, Log, InstigatorAsActor->GetActorLocation(), 50.0, FColor::Green, TEXT("%s"), *NewInstigator->GetName());
	}
	else
	{
		UE_VLOG_LOCATION(this, Bango, Log, this->GetActorLocation(), 50.0, FColor::Green, TEXT("Start by %s"), *NewInstigator->GetName());
	}
#endif
	
#if WITH_EDITOR
	bRunActionsSafetyGate = true;
#endif
	
	RunActions(NewInstigator, StartActions, (bUseStartTriggerDelay ? StartTriggerDelay : 0.0));
	
#if WITH_EDITOR
	bRunActionsSafetyGate = false;
#endif
}

void ABangoEvent::RunStopActions(UObject* NewInstigator)
{
#if ENABLE_VISUAL_LOG	
	if (AActor* InstigatorAsActor = Cast<AActor>(NewInstigator))
	{
		UE_VLOG_LOCATION(this, Bango, Log, this->GetActorLocation(), 50.0, FColor::Red, TEXT("Stop by"));
		UE_VLOG_SEGMENT(this, Bango, Log, this->GetActorLocation(), InstigatorAsActor->GetActorLocation(), FColor::Red, TEXT(""));
		UE_VLOG_LOCATION(this, Bango, Log, InstigatorAsActor->GetActorLocation(), 50.0, FColor::Red, TEXT("%s"), *NewInstigator->GetName());
	}
	else
	{
		UE_VLOG_LOCATION(this, Bango, Log, this->GetActorLocation(), 50.0, FColor::Red, TEXT("Stop by %s"), *NewInstigator->GetName());
	}
#endif
#if WITH_EDITOR
	bRunActionsSafetyGate = true;
#endif

	RunActions(NewInstigator, StopActions, (bUseStopTriggerDelay ? StopTriggerDelay : 0.0));
	
#if WITH_EDITOR
	bRunActionsSafetyGate = false;
#endif
}

void ABangoEvent::RunActions(UObject* NewInstigator, TArray<UBangoAction*>& Actions, double Delay)
{
	check(bRunActionsSafetyGate);
	
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
bool ABangoEvent::HasCurrentState(EBangoEventState State)
{
	return CurrentState.HasFlag(State);
}

void ABangoEvent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DebugUpdate();
}

void ABangoEvent::BeginDestroy()
{
	if (IsValid(GEngine))
	{
		UBangoEngineSubsystem* EngineSubsystem = GEngine->GetEngineSubsystem<UBangoEngineSubsystem>();

		if (IsValid(EngineSubsystem))
		{
			EngineSubsystem->UnregisterBangoEvent(this);
		}
	}
	
	Super::BeginDestroy();
}

void ABangoEvent::DebugUpdate()
{
	UpdateState();
}

FLinearColor ABangoEvent::GetDebugColor()
{
	const FLinearColor FrozenExpiredColor	(0.27, 0.30, 0.50, 1.00);
	const FLinearColor FrozenColor			(0.90, 0.95, 1.00, 1.00);
	const FLinearColor ExpiredColor			(0.27, 0.25, 0.23, 1.00);
	const FLinearColor ActiveColor			(0.60, 1.00, 0.60, 1.00);
	const FLinearColor NormalColor			(0.85, 0.95, 0.85, 1.00);

	if (CurrentState.HasFlag(EBangoEventState::Frozen | EBangoEventState::Expired))
	{
		return FrozenExpiredColor;
	}
	else if (CurrentState.HasFlag(EBangoEventState::Frozen))
	{
		return FrozenColor;
	}
	else if (CurrentState.HasFlag(EBangoEventState::Expired))
	{
		return ExpiredColor;
	}
	else if (CurrentState.HasFlag(EBangoEventState::Active))
	{
		return ActiveColor;
	}
	else
	{
		return NormalColor;	
	}
}

void ABangoEvent::UpdateState()
{
	CurrentState.SetFlag(EBangoEventState::Active, Instigators.Num() > 0);
	CurrentState.SetFlag(EBangoEventState::Frozen, GetIsFrozen());
	CurrentState.SetFlag(EBangoEventState::Expired, GetIsExpired());
}

bool NameIs(const FProperty* InProperty, FName Name)
{
	return (InProperty->GetFName() == Name);
}

bool ABangoEvent::CanEditChange(const FProperty* InProperty) const
{
	return true;
}

void ABangoEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
