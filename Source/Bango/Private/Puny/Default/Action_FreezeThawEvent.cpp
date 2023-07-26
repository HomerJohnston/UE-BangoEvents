// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Puny/Default/Action_FreezeThawEvent.h"

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
			UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));

			if (!IsValid(EventComponent))
			{
				UE_LOG(Bango, Error, TEXT("Attempted to freeze other event but no target event was set!"));
				break;
			}
			
			EventComponent->SetFrozen(true);
			break;
		}
		case EPunyFreezeThawEventAction::UnfreezeEvent:
		{
			UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));

			if (!IsValid(EventComponent))
			{
				UE_LOG(Bango, Error, TEXT("Attempted to unfreeze other event but no target event was set!"));
				break;
			}

			EventComponent->SetFrozen(false);
			break;
		}
		default:
		{
			break;
		}
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
	UPunyEventComponent* EventComponent = Cast<UPunyEventComponent>(TargetComponent.GetComponent(GetActor()));
	
	if (!IsValid(EventComponent))
	{
		return false;
	}
	
	return OnStart != EPunyFreezeThawEventAction::DoNothing || OnStop != EPunyFreezeThawEventAction::DoNothing;
}
#endif

