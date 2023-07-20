#include "Puny/PunyEventComponent.h"

#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Utility/BangoColor.h"
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
#if WITH_EDITORONLY_DATA

	AActor* OuterActor = Cast<AActor>(GetOuter());

	if (OuterActor)
	{
		Plunger = CreateEditorOnlyDefaultSubobject<UPunyPlungerComponent>("PlungerDisplay");
		Plunger->SetupAttachment(OuterActor->GetRootComponent());
	}
#endif
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

#if WITH_EDITORONLY_DATA
	if (IsValid(Plunger))
	{
		Plunger->SetSourceEvent(Event);
	}
#endif
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

	
	/*
	if (!IsValid(Plunger))
	{
		AActor* OuterActor = Cast<AActor>(GetOuter());

		if (OuterActor)
		{
			Plunger = NewObject<UPunyPlungerComponent>(this);
			Plunger->RegisterComponent();
			Plunger->SetSourceEvent(Event);
			OuterActor->AddInstanceComponent(Plunger);

			FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
			Plunger->AttachToComponent(OuterActor->GetRootComponent(), Rules);
		}
	}
	*/

	/*
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
	*/
}

void UPunyEventComponent::OnUnregister()
{
	/*
	if (IsValid(Plunger))
	{
		AActor* OuterActor = Cast<AActor>(GetOuter());
		OuterActor->RemoveInstanceComponent(Plunger);
		
		Plunger->UnregisterComponent();
		Plunger = nullptr;
	}
	*/
	
	Super::OnUnregister();
}

FLinearColor UPunyEventComponent::GetDisplayColor() const
{
	if (!IsValid(Event) || !IsValid(GetWorld()))
	{
		return BangoColor::Error;
	}

	UWorld* World = GetWorld();

	FLinearColor Color = (bUseCustomColor) ? CustomColor : Event->GetDisplayBaseColor();

	Event->ApplyColorEffects(Color);
	
	if (World->IsGameWorld())
	{
		//if (State.HasFlag(EBangoEventState::Expired))
		//{
		//	Color = BangoColorOps::DarkDesatColor(Color);
		//}
		//if (State.HasFlag(EBangoEventState::Frozen))
		//{
		//	Color = BangoColorOps::LightDesatColor(Color);
		//}
		
		return Color;
	}
	else if (GetWorld()->IsEditorWorld())
	{
		return Color; // GetStartsFrozen() ? BangoColorOps::LightDesatColor(Color) : Color;
	}
	else
	{
		return BangoColor::Error;
	}
}

void UPunyEventComponent::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	Super::PostLoadSubobjects(OuterInstanceGraph);
}

void UPunyEventComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
}

void UPunyEventComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPunyEventComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}
#endif
