#include "Bango/Core/BangoEventComponent.h"

#include "CanvasItem.h"
#include "SceneView.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Components/BangoPlungerComponent.h"
#include "Bango/Utility/BangoLog.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Bango/BangoTrigger_OLD.h"
#include "Bango/BangoAction.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Core/BangoEvent_Bang.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "BangoEditorTooling/BangoDebugTextEntry.h"
#include "BangoEditorTooling/BangoColors.h"
#include "Editor.h"
#endif

#if WITH_EDITORONLY_DATA
// TODO FText / localization
TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> UBangoEventComponent::BangoEventsShowFlag(TEXT("BangoEventsShowFlag"), true, EShowFlagGroup::SFG_Developer, FText(INVTEXT("Bango Events")));
#endif

#define RETURN_IF(X) if(X) { return; }
#define CONTINUE_IF(X) if(X) { continue; }

UBangoEventComponent::UBangoEventComponent()
{
	Event = CreateDefaultSubobject<UBangoEvent_Bang>("Test");

#if WITH_EDITORONLY_DATA
	RETURN_IF(IsTemplate());

	//RETURN_IF(!GetOwner());

	//RETURN_IF(GetOwner()->IsTemplate())

	PlungerComponent = CreateEditorOnlyDefaultSubobject<UBangoPlungerComponent>("BangoPlunger", true);
	PlungerComponent->SetupAttachment(this);

	PlungerComponent->SetCastShadow(false);
	PlungerComponent->SetHiddenInGame(true);
	PlungerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DisplayMeshComponent = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>("DisplayMesh", true);
	DisplayMeshComponent->SetupAttachment(this);
	DisplayMeshComponent->SetCastShadow(false);
	DisplayMeshComponent->SetHiddenInGame(true);
	DisplayMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
#endif
}

bool UBangoEventComponent::GetStartsFrozen() const
{
	return bStartFrozen;
}

UBangoEvent* UBangoEventComponent::GetEvent() const
{
	return Event;
}

bool UBangoEventComponent::GetIsDisabled() const
{
	return bDisable;
}

bool UBangoEventComponent::GetIsFrozen() const
{
	return bIsFrozen;
}

void UBangoEventComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bDisable)
	{
		UE_LOG(LogBango, Warning, TEXT("Event on %s is set to be disabled during play and will be destroyed."), *GetOwner()->GetName());

		GetWorld()->OnWorldBeginPlay.AddUObject(this, &ThisClass::DestroyOnBeginPlay);
		
		return;
	}
	
	if (!IsValid(Event))
	{
		UE_LOG(LogBango, Error, TEXT("UBangoEventComponent of <%s> has no event handler set!"), *GetOwner()->GetName());
		return;
	}

	Event->Init();

	SetFrozen(bStartFrozen, true);
	
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	
	for (UBangoAction* Action : Actions)
	{
		Action->Initialize();
		Action->BeginPlay();
	}

#if WITH_EDITOR
	Event->OnStateChange.BindUObject(this, &ThisClass::UpdatePlungerProxy);

	if (IsValid(DisplayMeshComponent))
	{
		DisplayMeshComponent->SetHiddenInGame(!DevSettings->GetShowEventsInGame());
		UpdateDisplayMesh();
	}
#endif
}

void UBangoEventComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!IsValid(Event))
	{
		return;
	}
	
	for (UBangoTrigger_OLD* Trigger : Triggers)
	{
		Trigger->UnregisterEvent(Event);
		Trigger->SetEnabled(false);
	}

	for (UBangoAction* Action : Actions)
	{
		Event->UnregisterAction(Action);
	}
	
	for (UBangoAction* Action : Actions)
	{
		Action->EndPlay(EndPlayReason);
	}

#if WITH_EDITOR
	PlungerComponent->DestroyComponent();

	DisplayMeshComponent->DestroyComponent();
#endif
	
	Super::EndPlay(EndPlayReason);
}

void UBangoEventComponent::DestroyOnBeginPlay()
{
	DestroyComponent();
}

void UBangoEventComponent::OnEventTriggered(UBangoEvent* TriggeredEvent, FBangoEventSignal Signal)
{
	OnEventTriggeredDelegate.Broadcast(this, Signal.Type, Signal.Instigator);
}

#if WITH_EDITOR
FText UBangoEventComponent::GetDisplayName()
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
#endif

void UBangoEventComponent::SetFrozen(bool bNewFrozen, bool bForceSet)
{
	if (!bForceSet && bIsFrozen == bNewFrozen)
	{
		return;
	}

	UE_LOG(LogBango, VeryVerbose, TEXT("UBangoEventComponent SetFrozen: %s"), (bNewFrozen ? TEXT("True") : TEXT("False")));

	bIsFrozen = bNewFrozen;

	switch (bIsFrozen)
	{
		case false:
		{
			for (UBangoTrigger_OLD* Trigger : Triggers)
			{
				Trigger->RegisterEvent(Event);
				Trigger->SetEnabled(true);
			}

			for (UBangoAction* Action : Actions)
			{
				Event->RegisterAction(Action);
			}
			break;
		}
		case true:
		{
			for (UBangoTrigger_OLD* Trigger : Triggers)
			{
				Trigger->UnregisterEvent(Event);
				Trigger->SetEnabled(false);
			}

			for (UBangoAction* Action : Actions)
			{
				Event->UnregisterAction(Action);
			}
			break;
		}
	}

#if WITH_EDITOR
	UpdatePlungerProxy();
#endif
}

void UBangoEventComponent::OnEventExpired(UBangoEvent* InEvent)
{
	if (bDestroyWhenExpired)
	{
		DestroyComponent(true);
	}
	
	if (!bDoNotFreezeWhenExpired)
	{
		SetFrozen(true);
	}
}

#if WITH_EDITOR
FLinearColor UBangoEventComponent::GetDisplayColor() const
{
	if (!IsValid(Event) || !IsValid(GetWorld()))
	{
		return Bango::Colors::Error;
	}

	if (bDisable)
	{
		return Bango::Colors::DarkGrey;
	}

	FLinearColor Color = (bUseCustomColor) ? CustomColor : Event->GetDisplayBaseColor();

	return Color;
}

void UBangoEventComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR

	RETURN_IF(IsTemplate());

	if (GetWorld()->IsEditorWorld())
	{
		FEditorDelegates::PreBeginPIE.AddUObject(this, &ThisClass::UnregisterDebugDraw);
		FEditorDelegates::PrePIEEnded.AddUObject(this, &ThisClass::ReregisterDebugDraw);
		
		if (!DebugDrawService_Editor.IsValid())
		{
			DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDrawEditor));
		}
	}

	if (GetWorld()->IsGameWorld())
	{
		if (!DebugDrawService_Game.IsValid())
		{
			DebugDrawService_Game = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDrawGame));
		}
	}

	UpdateDisplayMesh();
#endif
}

#if WITH_EDITOR
void UBangoEventComponent::UnregisterDebugDraw(const bool PIE)
{
	UDebugDrawService::Unregister(DebugDrawService_Editor);
	DebugDrawService_Editor.Reset();
}
#endif

#if WITH_EDITOR
void UBangoEventComponent::ReregisterDebugDraw(const bool PIE)
{
	if (!DebugDrawService_Editor.IsValid())
	{
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDrawEditor));
	}
}
#endif

void UBangoEventComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITOR

	RETURN_IF(IsTemplate());

	UDebugDrawService::Unregister(DebugDrawService_Editor);
	UDebugDrawService::Unregister(DebugDrawService_Game);

	DebugDrawService_Editor.Reset();
	DebugDrawService_Game.Reset();
#endif
}

bool UBangoEventComponent::CanEditChange(const FProperty* InProperty) const
{
	if (bDisable && InProperty->GetFName() != FName("bDisable"))
	{
		//return false;
	}
	
	return Super::CanEditChange(InProperty);
}

void UBangoEventComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropName = PropertyChangedEvent.GetPropertyName();
	FName MemberName = PropertyChangedEvent.GetMemberPropertyName();
	
	TArray MeshProps {"bUseDisplayMesh", "DisplayMesh", "DisplayMeshScale", "DisplayMeshOffset"};

	if (MeshProps.Contains(PropName))
	{
		UpdateDisplayMesh();
	}

	if (IsValid(PlungerComponent))
	{
		PlungerComponent->MarkRenderDynamicDataDirty();
	}
}

void UBangoEventComponent::UpdatePlungerProxy()
{
	PlungerComponent->MarkRenderDynamicDataDirty();
}

void UBangoEventComponent::UpdateDisplayMesh()
{
	if (!IsValid(DisplayMeshComponent))
	{
		return;
	}

	if (bUseDisplayMesh && IsValid(DisplayMesh))
	{
		DisplayMeshComponent->SetStaticMesh(DisplayMesh);
		DisplayMeshComponent->SetVisibility(true);
		DisplayMeshComponent->SetWorldScale3D(FVector(DisplayMeshScale));
		DisplayMeshComponent->SetRelativeLocation(FVector(0, 0, DisplayMeshOffsetBase + DisplayMeshOffset));
		
		return;
	}
	
	if (!bUseDisplayMesh)
	{
		DisplayMesh = nullptr;
	}

	DisplayMeshComponent->SetStaticMesh(nullptr);
	DisplayMeshComponent->SetVisibility(false);
}

void UBangoEventComponent::DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const
{
	double DistanceSqrd;
	FVector ScreenLocation;
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	const UWorld* World = GetWorld();
	
	RETURN_IF(!IsValid(World))
	RETURN_IF(!UBangoEventComponent::BangoEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags))
	RETURN_IF(World->IsGameWorld())
	RETURN_IF(!GetDebugTextScreenLocation(Canvas, ScreenLocation, DistanceSqrd));
	RETURN_IF(DistanceSqrd > FMath::Square(DevSettings->GetFarDisplayDistance()));
	
	for (UBangoTrigger_OLD* Trigger : Triggers)
	{
		CONTINUE_IF(!IsValid(Trigger));
		Trigger->DebugDraw(Canvas, PlayerController);
	}
	
	for (UBangoAction* Action : Actions)
	{
		CONTINUE_IF(!IsValid(Action));
		Action->DebugDraw(Canvas, PlayerController);
	}

	double Distance = FMath::Sqrt(DistanceSqrd);

	FCanvasTextItem HeaderText = GetDebugHeaderText(ScreenLocation, Distance);
	Canvas->DrawItem(HeaderText);
	
	if (Distance < DevSettings->GetNearDisplayDistance())
	{
		TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter = TDelegate<TArray<FBangoDebugTextEntry>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Editor);

		// I could have just use newlines in a single FCanvasTextItem but there's no way to set up text justification nicely in it, text is always left justified.
		// By setting up an array of individual items I can keep them all centre justified and manually offset each one.
		TArray<FCanvasTextItem> DataText = GetDebugFooterText(Canvas, ScreenLocation, DataGetter, Distance);

		for (FCanvasTextItem& Text : DataText)
		{
			Canvas->DrawItem(Text);	
		}
	}
}

void UBangoEventComponent::DebugDrawGame(UCanvas* Canvas, APlayerController* PlayerController) const
{
	double DistanceSqrd;
	FVector ScreenLocation;
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	const UWorld* World = GetWorld();
	
	RETURN_IF(!IsValid(World))
	RETURN_IF(!UBangoEventComponent::BangoEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags))
	RETURN_IF(World->IsGameWorld() && !DevSettings->GetShowEventsInGame())
	RETURN_IF(!GetDebugTextScreenLocation(Canvas, ScreenLocation, DistanceSqrd));
	RETURN_IF(DistanceSqrd > FMath::Square(DevSettings->GetFarDisplayDistance()));
	
	for (UBangoTrigger_OLD* Trigger : Triggers)
	{
		CONTINUE_IF(!IsValid(Trigger));
		Trigger->DebugDraw(Canvas, PlayerController);
	}
	
	for (UBangoAction* Action : Actions)
	{
		CONTINUE_IF(!IsValid(Action));
		Action->DebugDraw(Canvas, PlayerController);
	}

	double Distance = FMath::Sqrt(DistanceSqrd);

	FCanvasTextItem HeaderText = GetDebugHeaderText(ScreenLocation, Distance);
	Canvas->DrawItem(HeaderText);
	
	if (Distance < DevSettings->GetNearDisplayDistance())
	{
		TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter = TDelegate<TArray<FBangoDebugTextEntry>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Game);

		//TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter = TDelegate<TArray<FBangoDebugTextEntry>()>::CreateUObject(this, &ThisClass::GetDebugDataString_Editor);
			
		// I could have just use newlines in a single FCanvasTextItem but there's no way to set up text justification nicely in it, text is always left justified.
		// By setting up an array of individual items I can keep them all centre justified and manually offset each one.
		TArray<FCanvasTextItem> DataText = GetDebugFooterText(Canvas, ScreenLocation, DataGetter, Distance);

		for (FCanvasTextItem& Text : DataText)
		{
			Canvas->DrawItem(Text);
		}
	}
}

bool UBangoEventComponent::GetDebugTextScreenLocation(UCanvas* Canvas, FVector& ScreenLocation, double& DistSqrd) const
{
	// Settings
	double X, Y;
	
	FVector WorldCameraPos;
	FVector WorldCameraDir;

	// Validity Logic
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), WorldCameraPos, WorldCameraDir);

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	float DebugTextOffset = DebugTextOffsetBase + DebugTextOffsetSizeScaler * DevSettings->GetEventDisplaySize();
	
	FVector WorldDrawLocation = GetPlungerWorldLocation() + FVector(0, 0, DebugTextOffset);

	DistSqrd = FVector::DistSquared(WorldDrawLocation, WorldCameraPos);

	FVector VectorToWorldDrawLocation = WorldDrawLocation - WorldCameraPos;

	if ((VectorToWorldDrawLocation | WorldCameraDir) < 0.0)
	{
		return false;
	}

	ScreenLocation = Canvas->Project(WorldDrawLocation);

	return true;
}

FCanvasTextItem UBangoEventComponent::GetDebugHeaderText(const FVector& ScreenLocationCentre, double Distance) const
{
	// TODO can I get an actual font that doesn't fucking suck?
	UFont* TextFont = GEngine->GetLargeFont();

	TextFont->SetFontScalingFactor(2.f);
	TextFont->ScalingFactor = 2.0;
	
	FVector2D HeaderTextPos(ScreenLocationCentre.X, ScreenLocationCentre.Y - 8);

	FText Text;

	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		Text = DisplayName;
	}
	else
	{
		Text = FText::FromString(GetOwner()->GetActorLabel());
	}

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	
	float FarDisplayDistance = DevSettings->GetFarDisplayDistance();

	float LerpAlpha = FMath::Clamp((FarDisplayDistance - Distance) / (1.2 * FarDisplayDistance - FarDisplayDistance), 0, 1);
	float ColorAlpha = FMath::Lerp(0.0, 1.0, LerpAlpha);
	float OutlineAlpha = FMath::Lerp(0.0, 1.0, FMath::Square(LerpAlpha));

	FLinearColor HeaderBaseColor = HasInvalidData() ? Bango::Colors::Orange : FLinearColor::White;
	FLinearColor HeaderColor = HeaderBaseColor;
	FLinearColor OutlineColor = FLinearColor(0.0, 0.0, 0.0, OutlineAlpha);

	if (bUseDisplayName)
	{
		HeaderColor = Bango::Colors::LightBlue * HeaderBaseColor;
	}

	HeaderColor.A = ColorAlpha;
	
	FCanvasTextItem TextItem(HeaderTextPos, Text, TextFont, HeaderColor);
	TextItem.bCentreX = true;
	TextItem.bCentreY = true;
	TextItem.bOutlined = true;
	TextItem.OutlineColor = OutlineColor;
	TextItem.Scale = FVector2d(1.1, 1.1);

	return TextItem;
}

TArray<FCanvasTextItem> UBangoEventComponent::GetDebugFooterText(UCanvas* Canvas, const FVector& ScreenLocationCentre, TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter, double Distance) const
{
	UFont* TextFont = GEngine->GetLargeFont();

	FVector2D DataTextPos(ScreenLocationCentre.X, ScreenLocationCentre.Y + 8);

	TArray<FBangoDebugTextEntry> Data = DataGetter.Execute();

	TArray<FCanvasTextItem> CanvasTextItems;

	const double LineOffset = 16;
	double CurrentLineOffset = 0;
	
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();

	float NearDisplayDistance = DevSettings->GetNearDisplayDistance();
	
	float LerpAlpha = FMath::Clamp((NearDisplayDistance - Distance) / (1.2 * NearDisplayDistance - NearDisplayDistance), 0, 1);
	float ColorAlpha = FMath::Lerp(0.0, 1.0, LerpAlpha);
	float OutlineAlpha = FMath::Lerp(0.0, 1.0, FMath::Square(LerpAlpha));

	FLinearColor OutlineColor(0.05, 0.05, 0.05, OutlineAlpha);
	
	for(const FBangoDebugTextEntry& S : Data)
	{
		float LX = 0;
		float LY = 0;
		float RX = 0;
		float RY = 0;
		
		Canvas->StrLen(TextFont, S.TextL, LX, LY, false);
		Canvas->StrLen(TextFont, S.TextR, RX, RY, false);

		FLinearColor LColor = S.Color;
		LColor.A = ColorAlpha;
		
		FCanvasTextItem TextLeft(DataTextPos, FText::FromString(S.TextL), TextFont, LColor);
		TextLeft.Position.X -= LX;
		TextLeft.Position.Y += CurrentLineOffset;
		TextLeft.Scale = FVector2d(1.0);
		TextLeft.bOutlined = true;
		TextLeft.OutlineColor = OutlineColor;

		FLinearColor RColor = S.Color;
		RColor.A = ColorAlpha;
		
		FCanvasTextItem TextRight(DataTextPos, FText::FromString(S.TextR), TextFont, RColor);
		TextRight.Position.X += 3;
		TextRight.Position.Y += CurrentLineOffset;
		TextRight.Scale = FVector2d(1.0);
		TextRight.bOutlined = true;
		TextRight.OutlineColor = OutlineColor;
		
		CanvasTextItems.Add(TextLeft);
		CanvasTextItems.Add(TextRight);

		CurrentLineOffset += LineOffset;
	}
	
	return CanvasTextItems;
}

TArray<FBangoDebugTextEntry> UBangoEventComponent::GetDebugDataString_Editor() const
{
	TArray<FBangoDebugTextEntry> Data;

	FLinearColor NormalTextColor = Bango::Colors::White;
	FLinearColor ErrorTextColor = Bango::Colors::Orange;
	
	if (!IsValid(Event))
	{
		return Data;
	}
	
	if (Event->GetUsesActivateLimit())
	{
		Data.Add(FBangoDebugTextEntry("Activation Limit:", FString::Printf(TEXT("%i"), Event->GetActivateLimit()), NormalTextColor));
	}

	if (Triggers.IsEmpty())
	{
		Data.Add(FBangoDebugTextEntry("Triggers:", "NONE", ErrorTextColor));
	}
	
	for (UBangoTrigger_OLD* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{			
			Data.Add(FBangoDebugTextEntry("Trigger:", "NULL", ErrorTextColor));
			continue;
		}

		FString Prefix = "Trigger:";
			
		TStringBuilder<128> TriggerEntry;
		TriggerEntry.Append(Trigger->GetDisplayName().ToString());

		FLinearColor TextColor = (Trigger->HasValidSetup() ? NormalTextColor : ErrorTextColor);
		
		Data.Add(FBangoDebugTextEntry(Prefix, TriggerEntry.ToString(), TextColor));

		Trigger->AppendDebugData(Data);
	}

	if (Actions.IsEmpty())
	{
		Data.Add(FBangoDebugTextEntry("Action:", "NONE", ErrorTextColor));
	}
	
	for (UBangoAction* Action : Actions)
	{
		TStringBuilder<128> ActionEntry;

		if (!IsValid(Action))
		{
			Data.Add(FBangoDebugTextEntry("Action:", "NULL", ErrorTextColor));
			continue;
		}
		
		ActionEntry.Append(*Action->GetDisplayName().ToString());
		
		FLinearColor TextColor = (Action->HasValidSetup() ? NormalTextColor : ErrorTextColor);
		
		Data.Add(FBangoDebugTextEntry("Action:", ActionEntry.ToString(), TextColor));

		Action->AppendDebugData(Data);
	}	
	
	return Data;
}

TArray<FBangoDebugTextEntry> UBangoEventComponent::GetDebugDataString_Game() const
{
	TArray<FBangoDebugTextEntry> Data; 

	if (!IsValid(Event))
	{
		return Data;
	}

	if (Event->GetUsesActivateLimit())
	{
		int32 ActivateCount = Event->GetActivateCount();

		Data.Add(FBangoDebugTextEntry("Activations:", FString::Printf(TEXT("(%i/%i)"), ActivateCount, Event->GetActivateLimit())));
	}
	
	for (UBangoTrigger_OLD* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{			
			Data.Add(FBangoDebugTextEntry("Trigger:", "NULL TRIGGER", Bango::Colors::Orange));
		}
		else
		{
			FString Prefix = "Trigger:";
			
			TStringBuilder<128> TriggerEntry;

			TriggerEntry.Append(Trigger->GetDisplayName().ToString());

			Data.Add(FBangoDebugTextEntry(Prefix, TriggerEntry.ToString()));

			Trigger->AppendDebugData(Data);
		}
	}

	Event->AppendDebugDataString_Game(Data);

	return Data;
}

FVector UBangoEventComponent::GetPlungerWorldLocation() const
{
	if (!IsValid(PlungerComponent))
	{
		return GetOwner()->GetActorLocation();
	}

	return PlungerComponent->GetComponentLocation();
}

bool UBangoEventComponent::HasInvalidData() const
{	
	if (Triggers.IsEmpty())
	{
		return true;
	}

	for (UBangoTrigger_OLD* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{
			return true;
		}	
	}

	if (Actions.IsEmpty())
	{
		return true;
	}
	
	for(UBangoAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			return true;
		}
	}

	return false;
}
#endif

