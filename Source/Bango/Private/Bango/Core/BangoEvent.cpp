#include "Bango/Core/BangoEvent.h"

#include "Editor.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/TriggerCondition.h"
#include "Bango/Subsystems/BangoEngineSubsystem.h"
#include "Components/ShapeComponent.h"
#include "Editor/EditorEngine.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"


ABangoEvent::ABangoEvent()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

#if WITH_EDITOR
	DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>("DebugMesh");
	DebugMesh->SetupAttachment(RootComponent);
	//DebugMesh->SetHiddenInGame(true);
	DebugMesh->SetHiddenInSceneCapture(true);
#endif
}

void ABangoEvent::BeginPlay()
{
	Super::BeginPlay();
	
	for (UBangoTriggerCondition* Trigger : StartTriggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Setup(this);
			Trigger->OnTrigger.BindDynamic(this, &ThisClass::Activate);
		}
	}
	for (UBangoTriggerCondition* Trigger : StopTriggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Setup(this);
			Trigger->OnTrigger.BindDynamic(this, &ThisClass::Deactivate);
		}		
	}

#if WITH_EDITOR
	UpdateEditorVars();
	DebugUpdate();
#endif
}

void ABangoEvent::Activate(UObject* NewInstigator)
{	
	if (!bRunForEveryInstigator && (ActiveInstigators.Num() > 0 || DelayedInstigators.Num() > 0) || (bUseTriggerCountLimit && TriggerCount >= TriggerLimit))
	{
		// Ignore - it's already been or being triggered
		return;
	}
	if (bUseStartTriggerDelay)
	{
		AddDelayedInstigator(NewInstigator);
	}
	else
	{
		AddActiveInstigator(NewInstigator);
	}
}

void ABangoEvent::Deactivate(UObject* OldInstigator)
{
	if (!bRunForEveryInstigator && !bStopFromAnyInstigator && !ActiveInstigators.Contains(OldInstigator) && !DelayedInstigators.Contains(OldInstigator)) // TODO: is it a problem that the stop instigator could be different from the start instigator?
	{
		return;
	}
	
	if (bUseStartTriggerDelay && DelayedInstigators.Contains(OldInstigator))
	{
		RemoveDelayedInstigator(OldInstigator);
	}
	else
	{
		RemoveActiveInstigator(OldInstigator);
	}
}

void ABangoEvent::AddActiveInstigator(UObject* NewInstigator)
{
	// If the event doesn't have a stop trigger then we actually don't want to turn it on, we just want to bang off the start actions
	if (StopTriggers.Num() > 0)
	{
		ActiveInstigators.Add(NewInstigator);
	}
	
	for(UBangoAction* Action : StartActions)
	{
		Action->Run(this, NewInstigator);
	}

	TriggerCount++;
	
	Update();
}

void ABangoEvent::RemoveActiveInstigator(UObject* OldInstigator)
{
	if (ActiveInstigators.Contains(OldInstigator))
	{
		
	}
	
	for (UBangoAction* Action : StopActions)
	{
		Action->Run(this, OldInstigator);
	}
	
	ActiveInstigators.Remove(OldInstigator);
	Update();
}

void ABangoEvent::AddDelayedInstigator(UObject* NewInstigator)
{
	DelayedInstigators.Add(NewInstigator);
		
	FTimerHandle TimerHandle;

	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::FinishTriggerDelay, NewInstigator);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, StartTriggerDelay, false);

	DelayedTimers.Emplace(NewInstigator, TimerHandle);
	
	Update();
}

void ABangoEvent::FinishTriggerDelay(UObject* PendingInstigator)
{
	DelayedInstigators.Remove(PendingInstigator);
	AddActiveInstigator(PendingInstigator);
}

void ABangoEvent::RemoveDelayedInstigator(UObject* OldInstigator)
{
	DelayedInstigators.Remove(OldInstigator);
	
	FTimerHandle& TimerHandle = DelayedTimers[OldInstigator];

	GetWorldTimerManager().ClearTimer(TimerHandle);

	DelayedTimers.Remove(OldInstigator);

	Update();
}

void ABangoEvent::Update()
{
	if (bUseTriggerCountLimit && TriggerCount >= TriggerLimit)
	{
		Freeze();
	}
	
#if WITH_EDITOR
	DebugUpdate();
#endif
}

void ABangoEvent::Freeze()
{
	for (UBangoTriggerCondition* Trigger : StartTriggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Freeze(this);
		}
	}
	for (UBangoTriggerCondition* Trigger : StopTriggers)
	{
		if (IsValid(Trigger))
		{
			Trigger->Freeze(this);
		}		
	}
	
	bFrozen = true;
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
	
	UBangoEngineSubsystem* EngineSubsystem = GEngine->GetEngineSubsystem<UBangoEngineSubsystem>();
	EngineSubsystem->RegisterBangoEvent(this);
}

void ABangoEvent::UpdateState()
{
	CurrentStates.Empty((uint8)EBangoEventState::MAX);

	if (ActiveInstigators.Num() > 0)
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
	}
}

void ABangoEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	UpdateEditorVars();
}

void ABangoEvent::UpdateEditorVars()
{
	TArray<UShapeComponent*> Colliders;
	GetComponents(Colliders);

	NumCollisionVolumes = Colliders.Num();

	NumStopTriggers = StopTriggers.Num();

	bRunForEveryInstigatorSet = (bRunForEveryInstigator) ? 1 : 0;
}

void ABangoEvent::SetDebugMesh(UStaticMesh* Mesh)
{
	DebugMesh->SetStaticMesh(Mesh);
}

void ABangoEvent::SetDebugMeshMaterial(UMaterialInstance* MaterialInstance)
{
	int32 NumMaterials = DebugMesh->GetNumMaterials();

	for (int32 i = 0; i < NumMaterials; i++)
	{
		DebugMesh->SetMaterial(i, MaterialInstance);
	}
}

bool ABangoEvent::CanEditChange(const FProperty* InProperty) const
{
	return Super::CanEditChange(InProperty);
}
#endif
