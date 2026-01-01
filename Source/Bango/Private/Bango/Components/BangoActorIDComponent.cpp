#include "Bango/Components/BangoActorIDComponent.h"

#include "CanvasItem.h"
#include "Bango/Editor/BangoDebugUtility.h"
#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Fonts/FontMeasure.h"

UBangoActorIDComponent::UBangoActorIDComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
#if WITH_EDITORONLY_DATA 
	IconTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Bango/Icon_ActorID.Icon_ActorID"));
#endif
}

void UBangoActorIDComponent::PostLoad()
{
	Super::PostLoad();
	
#if WITH_EDITOR
	if (Bango::IsComponentInEditedLevel(this))
	{
		return;
	}
#endif
	
	//UBangoActorIDSubsystem::RegisterActor(this, ActorID, GetOwner());
}

void UBangoActorIDComponent::BeginPlay()
{
	Super::BeginPlay();

	UBangoActorIDSubsystem::RegisterActor(this, GetOwner(), Name, Guid);
}

void UBangoActorIDComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UBangoActorIDSubsystem::UnregisterActor(this, Guid);
	
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UBangoActorIDComponent::SetActorID(FName NewID)
{
	Modify();
	Name = NewID;
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnRegister()
{
	if (Bango::IsComponentInEditedLevel(this))
	{
		if (!Guid.IsValid())
		{
			Modify();
			Guid = FGuid::NewGuid();
		}
	}
	
	Super::OnRegister();
	
	BangoDebugDraw_Register<UBangoActorIDComponent>(this);
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnUnregister()
{
	BangoDebugDraw_Unregister(this);
	
	Super::OnUnregister();
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const
{
	UTexture* BackgroundTex = LoadObject<UTexture>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture"));
	UFont* Font = GEngine->GetLargeFont();
	
	const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TextSize = FontMeasureService->Measure(Name.ToString(), Font->GetLegacySlateFontInfo());
	
	FLinearColor TagColor = BangoColor::White;
	TagColor.A *= Alpha;
	
	float IconRawSize = 32.0f;
	float IconPadding = 4.0f;
	float IconScale = 0.5f;
	float Padding = 4.0f;
	float Border = 2.0f;
	
	float IconSize = IconRawSize * IconScale;
	
	{
		// Background Border
		float X = ScreenLocation.X - 0.5f * TextSize.X - Padding - 0.5f * IconSize - 0.5f * IconPadding;
		float Y = ScreenLocation.Y - 0.5f * FMath::Max(TextSize.Y, IconSize) - Padding;
		float XL = TextSize.X + 2.0 * Padding + IconSize + IconPadding;
		float YL = FMath::Max(TextSize.Y, IconSize) + 2.0 * Padding;
		FVector4f UV(0.0f, 0.0f, 1.0f, 1.0f);
	
		Canvas->SetDrawColor(FColor(0, 0, 0, 200 * Alpha));
		Canvas->DrawTile(BackgroundTex, X, Y, XL, YL, UV.X, UV.Y, UV.Z, UV.Z);
		
		Canvas->SetDrawColor(FColor(150, 150, 150, 150 * Alpha));
		Canvas->DrawTile(BackgroundTex, X - Border, Y - Border, XL + 2.0 * Border, YL + 2.0 * Border, UV.X, UV.Y, UV.Z, UV.Z);
	}
	
	{
		// ID Text
		float X = ScreenLocation.X + 0.5f * IconSize + 0.5f * IconPadding;
		float Y = ScreenLocation.Y - 0.5f;
		
		FCanvasTextItem Label(FVector2D(X, Y), FText::FromName(Name), Font, TagColor);
		Label.bCentreX = true;
		Label.bCentreY = true;
		Canvas->SetDrawColor(TagColor.ToFColor(false));
		Canvas->DrawItem(Label);
	}
	
	{
		// ID Icon
		float X = ScreenLocation.X - 0.5f * TextSize.X - 0.5f * IconSize - 0.5f * IconPadding;
		float Y = ScreenLocation.Y - 0.5f * IconSize;
		
		FCanvasIcon Icon = UCanvas::MakeIcon(IconTexture, 0.0f, 0.0f, IconRawSize, IconRawSize);
		Canvas->SetDrawColor(TagColor.ToFColor(false));
		Canvas->DrawIcon(Icon, X, Y, IconScale);
	}
}

void UBangoActorIDComponent::DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const
{
	
}
#endif

