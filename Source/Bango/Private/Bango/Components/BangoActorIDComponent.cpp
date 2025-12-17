#include "Bango/Components/BangoActorIDComponent.h"

#include "CanvasItem.h"
#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Fonts/FontMeasure.h"

UBangoActorIDComponent::UBangoActorIDComponent()
{
#if WITH_EDITORONLY_DATA 
	IconTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Bango/NameTag.NameTag"));
#endif
}

void UBangoActorIDComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UBangoActorIDComponent::PostLoad()
{
	Super::PostLoad();
	
	if (Bango::IsComponentInEditedLevel(this))
	{
		return;
	}
	
	UBangoActorIDSubsystem::RegisterActor(this, ActorID, GetOwner());
}

void UBangoActorIDComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBangoActorIDComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UBangoActorIDSubsystem::UnregisterActor(this, ActorID);
	
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UBangoActorIDComponent::SetActorID(FName NewID)
{
	Modify();
	ActorID = NewID;
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnRegister()
{
	Super::OnRegister();

	if (IsTemplate())
	{
		return;
	}

	if (GetWorld()->IsEditorWorld())
	{
		FEditorDelegates::PreBeginPIE.AddUObject(this, &ThisClass::UnregisterDebugDraw);
		FEditorDelegates::PrePIEEnded.AddUObject(this, &ThisClass::ReregisterDebugDraw);
		
		if (!DebugDrawService.IsValid())
		{
			DebugDrawService = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDrawEditor));
		}
	}
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::UnregisterDebugDraw(const bool PIE)
{
	UDebugDrawService::Unregister(DebugDrawService);
	DebugDrawService.Reset();
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::ReregisterDebugDraw(const bool PIE)
{
	if (!DebugDrawService.IsValid())
	{
		DebugDrawService = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDrawEditor));
	}
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnUnregister()
{
	Super::OnUnregister();
	
	if (IsTemplate())
	{
		return;
	}

	UDebugDrawService::Unregister(DebugDrawService);

	DebugDrawService.Reset();
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::DebugDrawEditor(UCanvas* Canvas, APlayerController* ALWAYS_NULL) const
{
	FVector ScreenLocation;
	
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	AActor* Actor = GetOwner();
	if (!IsValid(Actor)) return;

	double X, Y;
	
	FVector WorldCameraPos;
	FVector WorldCameraDir;

	// Validity Logic
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), WorldCameraPos, WorldCameraDir);

	const float MinDistance = 2000.0f;
	const float MaxDistance = 2500.0f;
	
	const float MinDistanceSqr = FMath::Square(MinDistance);
	const float MaxDistanceSqr = FMath::Square(MaxDistance);
	
	float DistanceSqr = FVector::DistSquared(WorldCameraPos, Actor->GetActorLocation());
	if (DistanceSqr > MaxDistanceSqr) return;

	float LerpAlpha = FMath::Clamp((DistanceSqr - MinDistanceSqr) / (MaxDistanceSqr - MinDistanceSqr), 0.0f, 1.0f);
	
	float Alpha = FMath::Lerp(1.0f, 0.0f, LerpAlpha);
	
	ScreenLocation = Canvas->Project(Actor->GetActorLocation() + FVector(0.f, 0.f, 150.f + LabelHeightAdjustment), false);
	if (ScreenLocation.Z < 0.0f) return;

	UTexture* BorderTex = LoadObject<UTexture>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture")); //WhiteSquareTexture
	UTexture* BackgroundTex = LoadObject<UTexture>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture"));

	UFont* Font = GEngine->GetLargeFont();
	const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TextSize = FontMeasureService->Measure(ActorID.ToString(), Font->GetLegacySlateFontInfo());

	FCanvasBoxItem Box(
		FVector2D(ScreenLocation.X - 0.5f * TextSize.X, ScreenLocation.Y - 0.5 * TextSize.Y),
		TextSize
		);

	FLinearColor TagColor = BangoColor::White;
	TagColor.A *= Alpha;
	
	Canvas->SetDrawColor(FColor(30, 30, 30, 150 * Alpha));
	Canvas->DrawTile(
		BackgroundTex,
		ScreenLocation.X - 0.5f * TextSize.X - /*10*/ 26, ScreenLocation.Y - 5,
		TextSize.X + /*20*/36, TextSize.Y + 10,
		0.0f,0.0f,1.0f,1.0f
		);
	//Canvas->DrawItem(Box);
	FCanvasTextItem Label(FVector2D(ScreenLocation), FText::FromName(ActorID), Font, TagColor);
	Label.bCentreX = true;
	Canvas->SetDrawColor(TagColor.ToFColor(false));
	Canvas->DrawItem(Label);

	
	FCanvasIcon Icon = UCanvas::MakeIcon(IconTexture, 0.0f, 0.0f, 24.0f, 32.0f);
	Canvas->SetDrawColor(TagColor.ToFColor(false));
	Canvas->DrawIcon(Icon, ScreenLocation.X - 34.0f, ScreenLocation.Y - 2.0f, 0.6f);
	
	/*
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
	*/
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	
	if (IsBeingEditorDeleted())
	{
		UE_LOG(LogBango, Warning, TEXT("Bango ID Component OnComponentDestroyed: %s"), *this->GetName());
	}
}

bool UBangoActorIDComponent::IsBeingEditorDeleted() const
{
	// 1. Must be in editor world
	if (GIsPlayInEditorWorld || GetWorld() == nullptr || GetWorld()->IsGameWorld())
		return false;

	// 2. Component must be explicitely removed from its owner
	if (GetOwner() && !GetOwner()->GetComponents().Contains(this))
		return true;

	// 3. NOT deleted because PIE ended
	if (GEditor && GEditor->PlayWorld != nullptr)
		return false;

	// 4. NOT deleted because a Blueprint reinstance/recompile replaced it
	if (HasAnyFlags(RF_Transient) && HasAnyFlags(RF_ClassDefaultObject) == false)
		return false;

	return false;
}
#endif
