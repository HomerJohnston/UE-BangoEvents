﻿#include "Bango/Core/BangoEventProcessor.h"

#include "Bango/Log.h"
#include "Bango/Core/BangoAction.h"
#include "Bango/Core/BangoEvent.h"

// BASE PROCESSOR
// ================================================================================================

ABangoEvent* UBangoEventProcessor::GetEvent() const
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoEventProcessor::StartActions(UObject* NewInstigator)
{
	for (UBangoAction* Action : GetEvent()->GetActions())
	{
		if (!IsValid(Action))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
			continue;
		}
			
		Action->Start(GetEvent(), NewInstigator);
	}
}

void UBangoEventProcessor::StopActions(UObject* OldInstigator)
{
	for (UBangoAction* Action : GetEvent()->GetActions())
	{
		if (!IsValid(Action))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
			continue;
		}

		Action->Stop();
	}
}

int32 UBangoEventProcessor::GetInstigatorsNum()
{
	return Instigators.Num();
}

// BANG PROCESSOR
// ================================================================================================

bool UBangoEventProcessor_Bang::ActivateFromTrigger(UObject* NewInstigator)
{
	StartActions(NewInstigator);

	return true;
}

bool UBangoEventProcessor_Bang::DeactivateFromTrigger(UObject* OldInstigator)
{
	StopActions(OldInstigator);
	
	return false;
}

// TOGGLE PROCESSOR
// ================================================================================================

bool UBangoEventProcessor_Toggle::ActivateFromTrigger(UObject* NewInstigator)
{
	if (Instigators.Contains(NewInstigator))
	{
		UE_LOG(Bango, Warning, TEXT("Warning: Instigator %s attempted to activate event %s, but is already registered as a running instigator!"), *NewInstigator->GetName(), *GetName());

		return false;
	}
		
	int32 InstigatorIndex = Instigators.Add(NewInstigator);

	if (InstigatorIndex == 0)
	{
		StartActions(NewInstigator);
	}

	return true;
}

bool UBangoEventProcessor_Toggle::DeactivateFromTrigger(UObject* OldInstigator)
{
	bool bRunStopActions;

	int32 InstigatorIndex = Instigators.Find(OldInstigator);
	
	switch (GetEvent()->GetDeactivateCondition())
	{
		case EBangoToggleDeactivateCondition::AllInstigatorsMustDeactivate:
		{
			bRunStopActions = Instigators.Num() == 1 && Instigators[0] == OldInstigator;
			break;
		}
		case EBangoToggleDeactivateCondition::AnyInstigatorCanDeactivate:
		{
			bRunStopActions = InstigatorIndex != INDEX_NONE;
			break;
		}
		case EBangoToggleDeactivateCondition::OriginalInstigatorDeactivates:
		{
			bRunStopActions = Instigators.Num() > 0 && Instigators[0] == OldInstigator;
			break;
		}
		default:
		{
			bRunStopActions = false;
			break;
		}
	}

	if (InstigatorIndex != INDEX_NONE)
	{
		Instigators.RemoveAt(InstigatorIndex);	
	}
	
	if (bRunStopActions)
	{
		StopActions(OldInstigator);

		return true;
	}

	return false;
}

// INSTANCED PROCESSOR
// ================================================================================================

void UBangoEventProcessor_Instanced::Initialize()
{
	
}

bool UBangoEventProcessor_Instanced::ActivateFromTrigger(UObject* NewInstigator)
{
	if (Instigators.Contains(NewInstigator))
	{
		UE_LOG(Bango, Warning, TEXT("Warning: Instigator %s attempted to activate event %s, but is already registered as a running instigator!"), *NewInstigator->GetName(), *GetName());

		return false;
	}
	
	Instigators.Add(NewInstigator);
		
	StartActions(NewInstigator);

	return true;
}

void UBangoEventProcessor_Instanced::StartActions(UObject* NewInstigator)
{
	// for an instanced event, we treat the assigned actions as template objects
	FBangoEventInstigatorActions NewInstancedActions;

	for (UBangoAction* Template : GetEvent()->GetActions())
	{
		if (!IsValid(Template))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *GetEvent()->GetName());
			continue;
		}
		
		// TODO pooling system?
		UBangoAction* ActionInstance = DuplicateObject(Template, this);
		NewInstancedActions.Actions.Add(ActionInstance);
	}

	FBangoEventInstigatorActions* InstancedActions = &InstancedActionsPerInstigator.Add(NewInstigator, NewInstancedActions);
	
	for (UBangoAction* InstancedAction : InstancedActions->Actions)
	{
		InstancedAction->Start(GetEvent(), NewInstigator);
	}
}

bool UBangoEventProcessor_Instanced::DeactivateFromTrigger(UObject* OldInstigator)
{
	StopActions(OldInstigator);

	Instigators.Remove(OldInstigator);

	return true;
}

void UBangoEventProcessor_Instanced::StopActions(UObject* OldInstigator)
{
	FBangoEventInstigatorActions* InstancedActions = InstancedActionsPerInstigator.Find(OldInstigator);

	if (!InstancedActions)
	{
		return;
	}

	for (UBangoAction* Action : InstancedActions->Actions)
	{
		if (!IsValid(Action))
		{
			UE_LOG(Bango, Warning, TEXT("Invalid action on event: %s, skipping"), *this->GetName());
			continue;
		}
			
		Action->Stop();
	}

	InstancedActionsPerInstigator.Remove(OldInstigator);
}
