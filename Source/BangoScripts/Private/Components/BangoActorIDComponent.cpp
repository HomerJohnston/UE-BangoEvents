#include "BangoScripts/Components/BangoActorIDComponent.h"

#include "CanvasItem.h"
#include "Editor.h"
#include "TimerManager.h"
#include "BangoScripts/EditorTooling/BangoDebugUtility.h"
#include "BangoScripts/Subsystem/BangoActorIDSubsystem.h"
#include "BangoScripts/EditorTooling/BangoColors.h"
#include "BangoScripts/EditorTooling/BangoHelpers.h"
#include "BangoScripts/Utility/BangoLog.h"
#include "Engine/Canvas.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"

UBangoActorIDComponent::UBangoActorIDComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
#if WITH_EDITORONLY_DATA 
	IconTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Bango/Icon_ActorID.Icon_ActorID"));
#endif
	
	if (!IsTemplate())
	{
		//PrintGuid("C++ Constructor");
		
#if WITH_EDITOR
		//EnsureValidGuid();
#endif	
	}
}

void UBangoActorIDComponent::PostInitProperties()
{
	Super::PostInitProperties();
	
	//PrintGuid("PostInitProperties");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::PostReinitProperties()
{
	Super::PostReinitProperties();
	
	//PrintGuid("PostReinitProperties");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	
	// Manual names should normally only be assigned at edit-time; if duplicating at game time, don't use a name
	if (!Bango::Editor::IsComponentInEditedLevel(this))
	{
		BangoName = NAME_None;
	}
}

void UBangoActorIDComponent::PostLoad()
{
	Super::PostLoad();
	
	//PrintGuid("PostLoad");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	Super::PostLoadSubobjects(OuterInstanceGraph);
	
	//PrintGuid("PostLoadSubobjects");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::PostLinkerChange()
{
	Super::PostLinkerChange();
	
	//PrintGuid("PostLinkerChange");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	
	//PrintGuid("Serialize");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

bool UBangoActorIDComponent::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags)
{
	bool Val = Super::Rename(NewName, NewOuter, Flags);
	
	//PrintGuid("Rename");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
	
	return Val;
}

void UBangoActorIDComponent::PostEditImport()
{
	Super::PostEditImport();
	
	//PrintGuid("PostEditImport");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::PostApplyToComponent()
{
	Super::PostApplyToComponent();
	
	//PrintGuid("PostApplyToComponent");
	
	//BangoUtility::Debug::PrintFlagNames();
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
}

void UBangoActorIDComponent::CheckForErrors()
{
	Super::CheckForErrors();
	
	//PrintGuid("CheckForErrors");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif	
}

void UBangoActorIDComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	//PrintGuid("InitializeComponent");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif	
}

void UBangoActorIDComponent::BeginPlay()
{
	Super::BeginPlay();

	//PrintGuid("BeginPlay");
	
	UBangoActorIDSubsystem::RegisterActor(this, GetOwner(), BangoName, BangoGuid);
}

void UBangoActorIDComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UBangoActorIDSubsystem::UnregisterActor(this, BangoGuid);
	
	Super::EndPlay(EndPlayReason);
}

void UBangoActorIDComponent::PrintGuid(const FString& FuncName)
{
	if (!Bango::Editor::IsComponentInEditedLevel(this))
	{
		return;
	}
	
	FString PrintedFuncName = FuncName;
	
	while (PrintedFuncName.Len() < 24)
	{
		PrintedFuncName += " ";
	}
	
	FString Msg = PrintedFuncName + " Guid: [" + BangoGuid.ToString() + "]";
	Bango::Debug::PrintComponentState(this, Msg);
}

#if WITH_EDITOR
void UBangoActorIDComponent::SetBangoName(FName NewID)
{
	Modify();
	BangoName = NewID;
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnRegister()
{
	Super::OnRegister();
	
	//PrintGuid("OnRegister");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif
	
#if WITH_EDITOR
	if (Bango::Editor::IsComponentInEditedLevel(this))
	{
		TWeakObjectPtr<UBangoActorIDComponent> WeakThis = this;
		
		auto FuckYouBostonStrangler = FTimerDelegate::CreateLambda( [WeakThis] ()
		{
			if (!WeakThis.IsValid())
			{
				return;
			}
			
			WeakThis->EnsureValidGuid();
		});
		
		GEditor->GetTimerManager()->SetTimerForNextTick(FuckYouBostonStrangler);
	}
	//BangoDebugDraw_Register<UBangoActorIDComponent>(this);
#endif
	
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::OnUnregister()
{
	//BangoDebugDraw_Unregister(this);
	
	Super::OnUnregister();
}

void UBangoActorIDComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	//PrintGuid("OnComponentCreated");
	
#if WITH_EDITOR
	//EnsureValidGuid();
#endif	
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::EnsureValidGuid()
{
#if 1
	if (Bango::Editor::IsComponentInEditedLevel(this))
	{
		if (!BangoGuid.IsValid() || BangoGuid == FGuid(0, 0, 0, 1))
		{
			UE_LOG(LogBango, Display, TEXT("EnsureValidGuid called - existing Guid: %s"), *BangoGuid.ToString());
			Modify();
			BangoGuid = FGuid::NewGuid();
			UE_LOG(LogBango, VeryVerbose, TEXT("Setting ID Component GUID to: %s"), *BangoGuid.ToString());
		}
	}
#endif
}
#endif

#if WITH_EDITOR
void UBangoActorIDComponent::DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const
{
	UTexture* BackgroundTex = LoadObject<UTexture>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture"));
	UFont* Font = GEngine->GetLargeFont();
	
	const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TextSize = FontMeasureService->Measure(BangoName.ToString(), Font->GetLegacySlateFontInfo());
	
	FLinearColor TagColor = Bango::Colors::White;
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
	
		Canvas->SetDrawColor(FColor(150, 150, 150, 150 * Alpha));
		Canvas->DrawTile(BackgroundTex, X - Border, Y - Border, XL + 2.0 * Border, YL + 2.0 * Border, UV.X, UV.Y, UV.Z, UV.Z);
		
		Canvas->SetDrawColor(FColor(20, 20, 20, 150 * Alpha));
		Canvas->DrawTile(BackgroundTex, X, Y, XL, YL, UV.X, UV.Y, UV.Z, UV.Z);
	}
	
	{
		// ID Text
		float X = ScreenLocation.X + 0.5f * IconSize + 0.5f * IconPadding;
		float Y = ScreenLocation.Y - 0.5f;
		
		FCanvasTextItem Label(FVector2D(X, Y), FText::FromName(BangoName), Font, TagColor);
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

