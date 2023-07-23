#include "Puny/PunyEventComponent.h"

#include "CanvasItem.h"
#include "Editor.h"
#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Utility/BangoColor.h"
#include "Puny/PunyPlungerComponent.h"
#include "Bango/Utility/Log.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Puny/PunyTrigger.h"
#include "Puny/PunyAction.h"
#include "Puny/PunyEvent.h"

#if WITH_EDITORONLY_DATA
// TODO FText
TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> UPunyEventComponent::PunyEventsShowFlag(TEXT("PunyEventsShowFlag"), true, EShowFlagGroup::SFG_Developer, FText(INVTEXT("Puny Events")));
#endif

#define RETURN_IF(X) if(X) { return; }
#define CONTINUE_IF(X) if(X) { continue; }

UPunyEventComponent::UPunyEventComponent()
{
#if WITH_EDITORONLY_DATA
	if (IsTemplate())
		return;
	
	PlungerComponent = CreateEditorOnlyDefaultSubobject<UPunyPlungerComponent>("PlungerDisplay");
	PlungerComponent->SetupAttachment(this);

	DisplayMeshComponent = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>("DisplayMesh");
	DisplayMeshComponent->SetupAttachment(this);
	DisplayMeshComponent->SetCastShadow(false);
	DisplayMeshComponent->SetHiddenInGame(true);
	DisplayMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
#endif
}

bool UPunyEventComponent::GetStartsFrozen() const
{
	return bStartFrozen;
}

UPunyEvent* UPunyEventComponent::GetEvent() const
{
	return Event;
}

bool UPunyEventComponent::GetIsFrozen() const
{
	return bIsFrozen;
}

void UPunyEventComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bDisable)
	{
		UE_LOG(Bango, Warning, TEXT("Event on %s is set to be disabled during play and will be destroyed."), *GetOwner()->GetName());

		GetWorld()->OnWorldBeginPlay.AddUObject(this, &ThisClass::DestroyOnBeginPlay);
		
		return;
	}
	
	if (!IsValid(Event))
	{
		UE_LOG(Bango, Error, TEXT("UPunyEventComponent of <%s> has no event handler set!"), *GetOwner()->GetName());
		return;
	}

	Event->Init();

	SetFrozen(bStartFrozen, true);
	
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();

	if (IsValid(DisplayMeshComponent))
	{
		DisplayMeshComponent->SetHiddenInGame(!DevSettings->GetShowEventsInGame());
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

void UPunyEventComponent::DestroyOnBeginPlay()
{
	if (PlungerComponent)
	{
		PlungerComponent->DestroyComponent();
	}

	if (DisplayMeshComponent)
	{
		DisplayMeshComponent->DestroyComponent();
	}
	
	DestroyComponent();
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
FLinearColor UPunyEventComponent::GetDisplayColor() const
{
	if (!IsValid(Event) || !IsValid(GetWorld()))
	{
		return BangoColor::Error;
	}

	if (bDisable)
	{
		return BangoColor::DarkGrey;
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

void UPunyEventComponent::OnRegister()
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
#endif
}

#if WITH_EDITOR
void UPunyEventComponent::UnregisterDebugDraw(const bool PIE)
{
	UDebugDrawService::Unregister(DebugDrawService_Editor);
	DebugDrawService_Editor.Reset();
}
#endif

#if WITH_EDITOR
void UPunyEventComponent::ReregisterDebugDraw(const bool PIE)
{
	if (!DebugDrawService_Editor.IsValid())
	{
		DebugDrawService_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDrawEditor));
	}
}
#endif

void UPunyEventComponent::OnUnregister()
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

bool UPunyEventComponent::CanEditChange(const FProperty* InProperty) const
{
	if (bDisable && InProperty->GetFName() != FName("bDisable"))
	{
		//return false;
	}
	
	return Super::CanEditChange(InProperty);
}

void UPunyEventComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropName = PropertyChangedEvent.GetPropertyName();
	FName MemberName = PropertyChangedEvent.GetMemberPropertyName();
	
	TArray MeshProps {"bUseDisplayMesh", "DisplayMesh", "DisplayMeshScale", "DisplayMeshOffset"};
	if (MeshProps.Contains(PropName))
	{
		UpdateDisplayMesh();
	}
}

void UPunyEventComponent::UpdateDisplayMesh()
{
	return;
	
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

void UPunyEventComponent::DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const
{
	double DistanceSqrd;
	FVector ScreenLocation;
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	const UWorld* World = GetWorld();
	
	RETURN_IF(!IsValid(World))
	RETURN_IF(!UPunyEventComponent::PunyEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags))
	RETURN_IF(World->IsGameWorld())
	RETURN_IF(!GetDebugTextScreenLocation(Canvas, ScreenLocation, DistanceSqrd));
	RETURN_IF(DistanceSqrd > FMath::Square(DevSettings->GetFarDisplayDistance()));
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		CONTINUE_IF(!IsValid(Trigger));
		Trigger->DebugDraw(Canvas, PlayerController);
	}
	
	for (UPunyAction* Action : Actions)
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

void UPunyEventComponent::DebugDrawGame(UCanvas* Canvas, APlayerController* PlayerController) const
{
	double DistanceSqrd;
	FVector ScreenLocation;
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	const UWorld* World = GetWorld();
	
	RETURN_IF(!IsValid(World))
	RETURN_IF(!UPunyEventComponent::PunyEventsShowFlag.IsEnabled(Canvas->SceneView->Family->EngineShowFlags))
	RETURN_IF(World->IsGameWorld() && !DevSettings->GetShowEventsInGame())
	RETURN_IF(!GetDebugTextScreenLocation(Canvas, ScreenLocation, DistanceSqrd));
	RETURN_IF(DistanceSqrd > FMath::Square(DevSettings->GetFarDisplayDistance()));
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		CONTINUE_IF(!IsValid(Trigger));
		Trigger->DebugDraw(Canvas, PlayerController);
	}
	
	for (UPunyAction* Action : Actions)
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

bool UPunyEventComponent::GetDebugTextScreenLocation(UCanvas* Canvas, FVector& ScreenLocation, double& DistSqrd) const
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

FCanvasTextItem UPunyEventComponent::GetDebugHeaderText(const FVector& ScreenLocationCentre, double Distance) const
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

	FLinearColor HeaderBaseColor = HasInvalidData() ? BangoColor::Orange : FLinearColor::White;
	FLinearColor HeaderColor = HeaderBaseColor;
	FLinearColor OutlineColor = FLinearColor(0.0, 0.0, 0.0, OutlineAlpha);

	if (bUseDisplayName)
	{
		HeaderColor = BangoColor::LightBlue * HeaderBaseColor;
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

TArray<FCanvasTextItem> UPunyEventComponent::GetDebugFooterText(UCanvas* Canvas, const FVector& ScreenLocationCentre, TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter, double Distance) const
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

TArray<FBangoDebugTextEntry> UPunyEventComponent::GetDebugDataString_Editor() const
{
	TArray<FBangoDebugTextEntry> Data;

	FLinearColor TextColor(1.0, 1.0, 1.0);
	
	if (!IsValid(Event))
	{
		return Data;
	}
	
	if (Event->GetUsesActivateLimit())
	{
		Data.Add(FBangoDebugTextEntry("Activation Limit:", FString::Printf(TEXT("%i"), Event->GetActivateLimit()), TextColor));
	}

	if (Triggers.IsEmpty())
	{
		Data.Add(FBangoDebugTextEntry("Triggers:", "NONE", BangoColor::Orange));
	}
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{			
			Data.Add(FBangoDebugTextEntry("Trigger:", "NULL", BangoColor::Orange));
			continue;
		}

		FString Prefix = "Trigger:";
			
		TStringBuilder<128> TriggerEntry;

		TriggerEntry.Append(Trigger->GetDisplayName().ToString());

		Data.Add(FBangoDebugTextEntry(Prefix, TriggerEntry.ToString(), TextColor));

		Trigger->AppendDebugData(Data);
	}

	if (Actions.IsEmpty())
	{
		Data.Add(FBangoDebugTextEntry("Action:", "NONE", BangoColor::Orange));
	}
	
	for (UPunyAction* Action : Actions)
	{
		TStringBuilder<128> ActionEntry;

		if (!IsValid(Action))
		{
			Data.Add(FBangoDebugTextEntry("Action:", "NULL", BangoColor::Orange));
			continue;
		}
		
		ActionEntry.Append(*Action->GetDisplayName().ToString());
		
		Data.Add(FBangoDebugTextEntry("Action:", ActionEntry.ToString(), TextColor));

		Action->AppendDebugData(Data);
	}	
	
	return Data;
}

TArray<FBangoDebugTextEntry> UPunyEventComponent::GetDebugDataString_Game() const
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
	
	for (UPunyTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{			
			Data.Add(FBangoDebugTextEntry("Trigger:", "NULL TRIGGER", BangoColor::Orange));
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

	return Data;
}

FVector UPunyEventComponent::GetPlungerWorldLocation() const
{
	if (!IsValid(PlungerComponent))
	{
		return GetOwner()->GetActorLocation();
	}

	return PlungerComponent->GetComponentLocation();
}

bool UPunyEventComponent::HasInvalidData() const
{	
	if (Triggers.IsEmpty())
	{
		return true;
	}

	for (UPunyTrigger* Trigger : Triggers)
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
	
	for(UPunyAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			return true;
		}
	}

	return false;
}
#endif
