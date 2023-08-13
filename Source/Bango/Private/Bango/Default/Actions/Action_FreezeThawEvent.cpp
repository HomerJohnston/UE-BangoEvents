// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Default/Actions/Action_FreezeThawEvent.h"

#include "Bango/Core/ActionSignal.h"
#include "Bango/Utility/Log.h"
#include "Bango/Core/EventComponent.h"
#include "Bango/Editor/DebugUtility.h"


UBangoAction_FreezeThawEvent::UBangoAction_FreezeThawEvent()
{
	OnStartAction = EBangoFreezeThawEventAction::DoNothing;
	OnStopAction = EBangoFreezeThawEventAction::DoNothing;
}

void UBangoAction_FreezeThawEvent::OnStart_Implementation(UBangoEvent* Event, UObject* Instigator)
{
	Handle(OnStartAction);
}

void UBangoAction_FreezeThawEvent::OnStop_Implementation(UBangoEvent* Event, UObject* Instigator)
{
	Handle(OnStopAction);
}

void UBangoAction_FreezeThawEvent::Handle(EBangoFreezeThawEventAction Action)
{
	switch (Action)
	{
		case EBangoFreezeThawEventAction::FreezeEvent:
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
		case EBangoFreezeThawEventAction::UnfreezeEvent:
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

void UBangoAction_FreezeThawEvent::HandleComponent(bool Val)
{
	UBangoEventComponent* EventComponent = Cast<UBangoEventComponent>(TargetComponent.GetComponent(GetActor()));

	if (!IsValid(EventComponent))
	{
		UE_LOG(Bango, Error, TEXT("Attempted to unfreeze other event but no target event was set!"));
		return;
	}

	EventComponent->SetFrozen(Val);
}

void UBangoAction_FreezeThawEvent::HandleActor(bool Val)
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::HandleActor - invalid Target Actor!"));
		return;
	}
	
	for (UActorComponent* Component : TargetActor->GetComponents())
	{
		if (!IsValid(Component))
		{
			return;
		}

		UBangoEventComponent* EventComponent = Cast<UBangoEventComponent>(Component);

		if (!IsValid(EventComponent))
		{
			continue;
		}

		EventComponent->SetFrozen(Val);
	}
}

#if WITH_EDITOR
void UBangoAction_FreezeThawEvent::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
	UBangoEventComponent* ActualTargetComponent = Cast<UBangoEventComponent>(TargetComponent.GetComponent(GetActor()));

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
void UBangoAction_FreezeThawEvent::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	Super::AppendDebugData(Data);
}
#endif

#if WITH_EDITOR
bool UBangoAction_FreezeThawEvent::HasValidSetup()
{
	if (bUseTargetComponent)
	{
		UBangoEventComponent* EventComponent = Cast<UBangoEventComponent>(TargetComponent.GetComponent(GetActor()));
	
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
		
		UBangoEventComponent* EventComponent = TargetActor->FindComponentByClass<UBangoEventComponent>();

		if (!IsValid(EventComponent))
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	return OnStartAction != EBangoFreezeThawEventAction::DoNothing || OnStopAction != EBangoFreezeThawEventAction::DoNothing;
}

void UBangoAction_FreezeThawEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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

