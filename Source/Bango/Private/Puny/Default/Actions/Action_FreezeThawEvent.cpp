// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Puny/Default/Actions/Action_FreezeThawEvent.h"

#include "Bango/Core/BangoSignal.h"
#include "Bango/Utility/Log.h"
#include "Puny/Core/EventComponent.h"
#include "Puny/Editor/DebugUtility.h"


UPunyAction_FreezeThawEvent::UPunyAction_FreezeThawEvent()
{
	OnStart = EPunyFreezeThawEventAction::DoNothing;
	OnStop = EPunyFreezeThawEventAction::DoNothing;
}

void UPunyAction_FreezeThawEvent::HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EPunyEventSignalType::StartAction:
		{
			Handle(OnStart);
			break;
		}
		case EPunyEventSignalType::StopAction:
		{
			Handle(OnStop);
			break;
		}
		default:
		{
			break;
		}
	}	
}

void UPunyAction_FreezeThawEvent::Handle(EPunyFreezeThawEventAction Action)
{
	switch (Action)
	{
		case EPunyFreezeThawEventAction::FreezeEvent:
		{
			if (bUseTargetComponent)
			{
				HandleComponent(true);
			}
			else if (bUseTargetActor)
			{
				HandleActor(true);
			}
			break;
		}
		case EPunyFreezeThawEventAction::UnfreezeEvent:
		{
			if (bUseTargetComponent)
			{
				HandleComponent(false);
			}
			else if (bUseTargetActor)
			{
				HandleActor(false);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void UPunyAction_FreezeThawEvent::HandleComponent(bool Val)
{
	UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));

	if (!IsValid(EventComponent))
	{
		UE_LOG(Bango, Error, TEXT("Attempted to unfreeze other event but no target event was set!"));
		return;
	}

	EventComponent->SetFrozen(Val);
}

void UPunyAction_FreezeThawEvent::HandleActor(bool Val)
{
	for (UActorComponent* Component : TargetActor->GetComponents())
	{
		if (!IsValid(Component))
		{
			return;
		}

		UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(Component);

		if (!IsValid(EventComponent))
		{
			continue;
		}

		EventComponent->SetFrozen(Val);
	}
}

#if WITH_EDITOR
void UPunyAction_FreezeThawEvent::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	UPunyEventComponent* ActualTargetComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));

	if (IsValid(ActualTargetComponent))
	{
		if (ActualTargetComponent->GetOwner() == GetActor())
		{
			DrawDebugCircle(GetWorld(), ActualTargetComponent->GetComponentTransform().ToMatrixNoScale(), 25.f, 8, FColor::Green);
		}
		else
		{
			BangoUtility::DebugDraw::DebugDrawDashedLine(GetWorld(), GetEventComponent()->GetComponentLocation(), ActualTargetComponent->GetComponentLocation(), 100.0f, FColor::Green);
		}
	}
}
#endif

#if WITH_EDITOR
void UPunyAction_FreezeThawEvent::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	Super::AppendDebugData(Data);
}
#endif

#if WITH_EDITOR
bool UPunyAction_FreezeThawEvent::HasValidSetup()
{
	if (bUseTargetComponent)
	{
		UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));
	
		if (!IsValid(EventComponent))
		{
			return false;
		}	
	}
	else if (bUseTargetActor)
	{
		if (!IsValid(TargetActor))
		{
			return false;
		}
		
		UPunyEventComponent* EventComponent = TargetActor->FindComponentByClass<UPunyEventComponent>();

		if (!IsValid(EventComponent))
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	return OnStart != EPunyFreezeThawEventAction::DoNothing || OnStop != EPunyFreezeThawEventAction::DoNothing;
}

void UPunyAction_FreezeThawEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	TArray<FName> TargetActorProperties {"bUseTargetActor", "TargetActor"};
	TArray<FName> TargetComponentProperties {"bUseTargetComponent", "TargetComponent"};
	
	// Ensure that only one setting is in use
	if (bUseTargetActor && bUseTargetComponent)
	{
		if (TargetActorProperties.Contains(PropertyChangedEvent.Property->GetFName()))
		{
			bUseTargetComponent = false;
		}
		else if (TargetComponentProperties.Contains(PropertyChangedEvent.Property->GetFName()))
		{
			bUseTargetActor = false;
		}
		else
		{
			bUseTargetActor = false;
			bUseTargetComponent = false;
		}
	}
}
#endif

