#include "Bango/Components/BangoScriptComponent.h"

#include "Bango/Core/BangoScript.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "Bango"

UBangoScriptComponent::UBangoScriptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
#if WITH_EDITORONLY_DATA 
	IconTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Bango/Icon_Script.Icon_Script"));
	LabelOffset = -48.0f;
#endif
}

void UBangoScriptComponent::PrintState(FString Msg) const
{	
	uint32 Flags = (uint32)GetOwner()->GetFlags();
	uint32 IntFlags = (uint32)GetOwner()->GetInternalFlags();
	
	FString BitString1;
	FString BitString2;
	BitString1.Reserve(32);
	BitString2.Reserve(32);

	while (Msg.Len() < 32)
	{
		Msg += " ";
	}
	
	for (int32 i = 31; i >= 0; --i)
	{
		BitString1.AppendChar(((Flags >> i) & 1) ? TEXT('1') : TEXT('0'));
		BitString2.AppendChar(((IntFlags >> i) & 1) ? TEXT('1') : TEXT('0'));
	}
	
	UE_LOG(LogBango, Display, TEXT("%s"), *FString::Format(TEXT("{0}: {1} | {2}"), { Msg, *BitString1, *BitString2 } ));
}

void UBangoScriptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bRunOnBeginPlay)
	{	
		Run();
	}
}

#if WITH_EDITOR
void UBangoScriptComponent::OnRegister()
{
	Super::OnRegister();
	
	BangoDebugDraw_Register<ThisClass>(this);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::OnUnregister()
{
	BangoDebugDraw_Unregister(this);

	Super::OnUnregister();
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	// PrintState("OnComponentCreated");
	
	// With RF_LoadCompleted this is a default actor component.
	
	// Because UE creates and destroys components a thousand times a second, we will not use this.
	// We will instead create the script using the details customization on the script container. This is probably more reliable anyway.  
	if (HasAllFlags(RF_LoadCompleted))
	{
		/*
		if (this->GetWorld() && this->ComponentIsInPersistentLevel(false))
		{
			auto Lambda = FTimerDelegate::CreateLambda([this] ()
			{
				FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &Script);
			});
			this->GetWorld()->GetTimerManager().SetTimerForNextTick(Lambda);
		}
		*/
		return;
	}
	
	if (!Bango::IsComponentInEditedLevel(this))
	{
		return;
	}
	
	// If it already has a Guid, it must have been a copy-paste.
	if (Script.GetGuid().IsValid())
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &Script);
	}
	else
	{
		FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &Script);
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	// PrintState("OnComponentDestroyed");
	
	// This flag seems to be set when the editor destroys the component, e.g. it is unloaded by world partition. It isn't set when you delete the component. 	
	if (HasAllFlags(RF_BeginDestroyed))
	{
		Super::OnComponentDestroyed(bDestroyingHierarchy);
		return;
	}
		
	// If we are a default actor component, we will always exist on the actor and the only time we'll be truly deleted is when the whole actor hierachy is being deleted
	if (CreationMethod != EComponentCreationMethod::Instance && !bDestroyingHierarchy)
	{
		Super::OnComponentDestroyed(bDestroyingHierarchy);
		return;
	}
	
	if (Bango::IsComponentInEditedLevel(this))
	{
		if (Script.GetGuid().IsValid())
		{
			// Moves handling over to an editor module
			FBangoEditorDelegates::OnScriptContainerDestroyed.Broadcast(this, &Script);
		}
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	if (Bango::IsComponentInEditedLevel(this))
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &Script);
	}
}

void UBangoScriptComponent::PostApplyToComponent()
{
	Super::PostApplyToComponent();
	
	// If it already has a Guid, it must have been a copy-paste.
	if (Script.GetGuid().IsValid())
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &Script);
	}
	else
	{
		FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &Script);
	}
}

#endif

#if WITH_EDITOR
void UBangoScriptComponent::UnsetScript()
{
	Modify();
	Script.Unset();
	
	if (!MarkPackageDirty())
	{
		UE_LOG(LogBlueprint, Error, TEXT("Could not mark the actor package dirty?"));
	}
}

void UBangoScriptComponent::OnRename()
{
	if (HasAnyFlags(RF_MirroredGarbage))
	{
		return;
	}
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(Script.GetScriptClass());

	if (Blueprint && Blueprint->GetName() == GetName())
	{
		Blueprint->UpdateAutoName(this);
	}
}
#endif

void UBangoScriptComponent::Run()
{
#if WITH_EDITOR
	RunningHandle = 
#endif
	UBangoScriptSubsystem::EnqueueScript(Script.GetScriptClass(), GetOwner());
	
#if WITH_EDITOR
	if (RunningHandle.IsRunning())
	{
		TDelegate<void(FBangoScriptHandle)> OnFinished = TDelegate<void(FBangoScriptHandle)>::CreateUObject(this, &ThisClass::OnScriptFinished);
		UBangoScriptSubsystem::RegisterOnScriptFinished(this, RunningHandle, OnFinished);
	}
#endif
}

#if WITH_EDITOR
FGuid UBangoScriptComponent::GetScriptGuid() const
{
	return Script.GetGuid();
}
#endif

#if WITH_EDITOR
UBangoScriptBlueprint* UBangoScriptComponent::GetScriptBlueprint() const
{
	return Cast<UBangoScriptBlueprint>(UBlueprint::GetBlueprintFromClass(Script.GetScriptClass().LoadSynchronous()));
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::SetScriptBlueprint(UBangoScriptBlueprint* Blueprint)
{
	if (!Blueprint->GeneratedClass->IsChildOf(UBangoScript::StaticClass()))
	{
		UE_LOG(LogBango, Error, TEXT("Tried to set blueprint but the blueprint given was not a UBangoScriptInstance!"));
		return;
	}
	
	Modify();
	(void)MarkPackageDirty();
	
	Script.SetScriptClass(Cast<UClass>(Blueprint->GeneratedClass));
}

void UBangoScriptComponent::OnScriptFinished(FBangoScriptHandle FinishedHandle)
{
	if (FinishedHandle != RunningHandle)
	{
		return;
	}
	
	RunningHandle.Expire();
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const
{
	FLinearColor TagColor = BangoColor::White;

	if (GetWorld()->IsGameWorld())
	{
		if (RunningHandle.IsExpired())
		{
			TagColor = BangoColor::YellowBase;
		}
		else if (RunningHandle.IsRunning())
		{
			TagColor = BangoColor::LightBlue;
		}
		else if (RunningHandle.IsNull())
		{
			TagColor = FLinearColor::Black;
		}
	}
	else
	{
		if (bRunOnBeginPlay)
		{
			TagColor = BangoColor::Green;
		}
	}
	
	FText LabelText = FText::FromString(Description);
	FVector2D TextSize = FVector2D::ZeroVector;
	UFont* Font = GEngine->GetLargeFont();
	
	TagColor.A *= Alpha;
	
	float IconRawSize = 32.0f;
	float IconScale = 0.5f;
	
	float IconSize = IconRawSize * IconScale;
	float Padding = 4.0f;
	float Border = 2.0f;
	float IconPadding = 4.0f;
	
	if (!LabelText.IsEmpty())
	{
		const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		TextSize = FontMeasureService->Measure(LabelText.ToString(), Font->GetLegacySlateFontInfo());
	}

	float TotalWidth = IconSize;
	float TotalHeight = FMath::Max(IconSize, IconSize);
	
	if (TextSize.X > KINDA_SMALL_NUMBER)
	{
		TotalWidth += IconPadding + TextSize.X;
	}
	
	{
		float X = ScreenLocation.X - 0.5f * TotalWidth - Padding;
		float Y = ScreenLocation.Y - 0.5f * TotalHeight - Padding;
		float XL = TotalWidth + 2.0f * Padding;
		float YL = TotalHeight + 2.0f * Padding;
		
		FVector4f UV(0.0f, 0.0f, 1.0f, 1.0f);
	
		UTexture* BackgroundTex = LoadObject<UTexture>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture"));
		
		Canvas->SetDrawColor(FColor(150, 150, 150, 150 * Alpha));
		Canvas->DrawTile(BackgroundTex, X - Border, Y - Border, XL + 2.0 * Border, YL + 2.0 * Border, UV.X, UV.Y, UV.Z, UV.Z);
		
		Canvas->SetDrawColor(FColor(20, 20, 20, 150 * Alpha));
		Canvas->DrawTile(BackgroundTex, X, Y, XL, YL, UV.X, UV.Y, UV.Z, UV.Z);
	}
	
	{
		// ID Icon
		float X = ScreenLocation.X - 0.5f * TotalWidth;
		float Y = ScreenLocation.Y - 0.5f * IconSize;
		
		FCanvasIcon Icon = UCanvas::MakeIcon(IconTexture, 0.0f, 0.0f, IconRawSize, IconRawSize);
		Canvas->SetDrawColor(TagColor.ToFColor(false));
		Canvas->DrawIcon(Icon, X, Y, IconScale);
	}
	
	if (!LabelText.IsEmpty())
	{
		// Text
		float X = ScreenLocation.X - 0.5f * TotalWidth + 0.5f * IconSize + 0.5f * IconPadding;
		float Y = ScreenLocation.Y;
		
		FCanvasTextItem Text(FVector2D(X + 0.5f * IconSize + IconPadding, Y), LabelText, Font, BangoColor::White);
		Text.bCentreY = true;
		Canvas->DrawItem(Text);
	}
}

void UBangoScriptComponent::DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const
{
	DebugDrawEditor(Canvas, ScreenLocation, Alpha);
}
#endif

#undef LOCTEXT_NAMESPACE