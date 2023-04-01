﻿#include "Bango/Core/BangoEvent.h"

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

// ============================================================================================
// Constructor
// ============================================================================================
ABangoEvent::ABangoEvent()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	PlungerComponent = CreateEditorOnlyDefaultSubobject<UBangoPlungerComponent>("Plunger");
	PlungerComponent->SetupAttachment(RootComponent);

	if (!IsRunningCommandlet())
	{
		//PlungerComponent->
	}
#endif
}

// ------------------------------------------
// Settings Getters
// ------------------------------------------
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
	UpdateEditorVars();
	DebugUpdate();
#endif
	
	Super::BeginPlay();
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
		RunActions(NewInstigator, StartActions);
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
	
	bool bRun = true;

	if (!RunStateSettings.bRunForEveryInstigator && Index != 0)
	{
		bRun =  false;
	}

	if (RunStateSettings.bRunForEveryInstigator && RunStateSettings.bQueueInstigators && Index != 0)
	{
		bRun = false;
	}
	
	if (bRun)
	{
		RunActions(OldInstigator, StopActions);
	}

	Instigators.RemoveAt(Index);

	// If it's a queued event, and if we just removed the first instigator, run the next instigator 
	if (RunStateSettings.bRunForEveryInstigator && RunStateSettings.bQueueInstigators && Index == 0 && Instigators.Num() > 0)
	{
		RunActions(Instigators[0], StartActions);
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

void ABangoEvent::Update()
{
}

void ABangoEvent::RunActions(UObject* NewInstigator, TArray<UBangoAction*>& Actions)
{
	for (UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s"), *this->GetName());
			continue;
		}
		
		Action->Run(this, NewInstigator);
	}
}

// Editor ---------------------------------------

#if WITH_EDITOR
bool ABangoEvent::HasCurrentState(EBangoEventState State)
{
	return CurrentStates.Contains(State);
}

void ABangoEvent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateEditorVars();

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

void ABangoEvent::UpdateState()
{
	/*CurrentStates.Empty((uint8)EBangoEventState::MAX);

	if (Instigators.Num() > 0)
	{
		CurrentStates.Add(EBangoEventState::Active);
	}
	
	if (bFrozen)
	{
		CurrentStates.Add(EBangoEventState::Frozen);
	}
	
	if (TriggerCount >= TriggerLimit)
	{
		CurrentStates.Add(EBangoEventState::Expired);
	}
	
	if (DelayedInstigators.Num() > 0)
	{
		CurrentStates.Add(EBangoEventState::StartDelay);
	}*/
}

bool NameIs(const FProperty* InProperty, FName Name)
{
	return (InProperty->GetFName() == Name);
}

bool ABangoEvent::CanEditChange(const FProperty* InProperty) const
{
	return true;
}



void ABangoEvent::UpdateEditorVars()
{
	TArray<UShapeComponent*> Colliders;
	GetComponents(Colliders);

	NumCollisionVolumes = Colliders.Num();

	NumStopTriggers = StopTriggers.Num();

	bRunForEveryInstigatorSet = (RunStateSettings.bRunForEveryInstigator) ? 1 : 0;

	bUseTriggerLimitSet = (bUseTriggerLimit) ? 1 : 0;
}

void ABangoEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	UpdateEditorVars();
}
#endif
