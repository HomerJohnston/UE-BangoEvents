#include "Puny/PunyEventComponent.h"

#include "Bango/Settings/BangoDevSettings.h"
#include "Puny/PunyPlungerComponent.h"
#include "Bango/Utility/Log.h"
#include "Puny/PunyTrigger.h"
#include "Puny/PunyAction.h"
#include "Puny/PunyEvent.h"
#include "Puny/PunyEventSignal.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyEvent_Bang.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

UPunyEventComponent::UPunyEventComponent()
{
}

void UPunyEventComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(Event))
	{
		UE_LOG(Bango, Error, TEXT("UPunyEventComponent of <%s> has no event handler set!"), *GetOwner()->GetName());
		return;
	}

	Event->Init();
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		Trigger->RegisterEvent(Event);
		Trigger->SetEnabled(true);
	}

	for (UPunyAction* Action : Actions)
	{
		Event->RegisterAction(Action);
	}
	
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();

	if (IsValid(OverrideDisplayMesh))
	{
		OverrideDisplayMesh->SetHiddenInGame(!DevSettings->GetShowEventsInGame());
	}
}

void UPunyEventComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!IsValid(Event))
	{
		UE_LOG(Bango, Error, TEXT("UPunyEventComponent of <%s> has no event handler set!"), *GetOwner()->GetName());
		return;
	}
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		Trigger->UnregisterEvent(Event);
		Trigger->SetEnabled(false);
	}

	for (UPunyAction* Action : Actions)
	{
		Event->UnregisterAction(Action);
	}
	
	Super::EndPlay(EndPlayReason);
}

FText UPunyEventComponent::GetDisplayName()
{
	if (bUseDisplayName)
	{
		if (DisplayName.IsEmpty())
		{
			return FText::FromString("UNNAMED");
		}
		
		return DisplayName;
	}
	
	return FText::FromString(GetOwner()->GetActorNameOrLabel());
}

#if WITH_EDITOR
void UPunyEventComponent::OnRegister()
{
	Super::OnRegister();

	if (!IsValid(Plunger))
	{
		AActor* OuterActor = Cast<AActor>(GetOuter());

		if (OuterActor)
		{
			Plunger = NewObject<UPunyPlungerComponent>(this);
			Plunger->RegisterComponent();
			Plunger->SetSourceEvent(Event);

			FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
			Plunger->AttachToComponent(OuterActor->GetRootComponent(), Rules);
		}
	}

	if (bUseCustomMesh && IsValid(CustomMesh))
	{
		if (!IsValid(OverrideDisplayMesh))
		{
			AActor* OuterActor = Cast<AActor>(GetOuter());

			if (OuterActor)
			{
				OverrideDisplayMesh = NewObject<UStaticMeshComponent>(this);

				OverrideDisplayMesh->SetCastShadow(false);
				OverrideDisplayMesh->SetHiddenInGame(true);
				OverrideDisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				OverrideDisplayMesh->RegisterComponent();

				FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
				OverrideDisplayMesh->AttachToComponent(OuterActor->GetRootComponent(), Rules);
			}
		}
	}
}

void UPunyEventComponent::OnUnregister()
{
	if (IsValid(Plunger))
	{
		Plunger->UnregisterComponent();
		Plunger = nullptr;
	}
	
	Super::OnUnregister();
}

FLinearColor UPunyEventComponent::GetDisplayColor() const
{
	if (!IsValid(Event))
	{
		return FColor::Magenta;
	}
	
	return Event->GetDisplayColor();
}
#endif
