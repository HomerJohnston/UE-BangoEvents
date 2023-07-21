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

bool UPunyEventComponent::GetStartsFrozen() const
{
	return bStartFrozen;
}

bool UPunyEventComponent::GetIsFrozen() const
{
	return bIsFrozen;
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

	SetFrozen(bStartFrozen, true);
	
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

void UPunyEventComponent::SetFrozen(bool bNewFrozen, bool bForceSet)
{
	if (!bForceSet && bIsFrozen == bNewFrozen)
	{
		return;
	}

	UE_LOG(Bango, Display, TEXT("UPunyEventComponent SetFrozen: %s"), (bNewFrozen ? TEXT("True") : TEXT("False")));

	bIsFrozen = bNewFrozen;

	switch (bIsFrozen)
	{
		case false:
		{
			for (UPunyTrigger* Trigger : Triggers)
			{
				Trigger->RegisterEvent(Event);
				Trigger->SetEnabled(true);
			}

			for (UPunyAction* Action : Actions)
			{
				Event->RegisterAction(Action);
			}
			break;
		}
		case true:
		{
			for (UPunyTrigger* Trigger : Triggers)
			{
				Trigger->UnregisterEvent(Event);
				Trigger->SetEnabled(false);
			}

			for (UPunyAction* Action : Actions)
			{
				Event->UnregisterAction(Action);
			}
			break;
		}
	}
}

void UPunyEventComponent::OnEventExpired(UPunyEvent* InEvent)
{
	if (!bDoNotFreezeWhenExpired)
	{
		SetFrozen(true);
	}
}

#if WITH_EDITOR
void UPunyEventComponent::OnRegister()
{
	Super::OnRegister();
}

void UPunyEventComponent::OnUnregister()
{	
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
		if (Event->GetIsExpired())
		{
			Color = BangoColorOps::DarkDesatColor(Color);
		}
		if (GetIsFrozen())
		{
			Color = BangoColorOps::LightDesatColor(Color);
		}
		
		return Color;
	}
	else if (GetWorld()->IsEditorWorld())
	{
		return GetStartsFrozen() ? BangoColorOps::LightDesatColor(Color) : Color;
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
