#include "Bango/Components/BangoScriptComponent.h"

#include "Bango/Core/BangoScript.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "BangoEditorTooling/BangoColors.h"
#include "BangoEditorTooling/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditorTooling/BangoDebugDrawCanvas.h"
#include "BangoEditorTooling/BangoDebugUtility.h"
#include "Components/BillboardComponent.h"
#include "Fonts/FontMeasure.h"
#include "UObject/ICookInfo.h"
#include "BangoEditorTooling/BangoEditorDelegates.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "Bango"

// ----------------------------------------------

UBangoScriptComponent::UBangoScriptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA 
	{
		FCookLoadScope EditorOnlyLoadScope(ECookLoadType::EditorOnly);
		IconTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Bango/Icon_Script.Icon_Script"));
	}
	
	// LabelOffset = -48.0f;
#endif
}

// ----------------------------------------------

void UBangoScriptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bRunOnBeginPlay)
	{	
		Run();
	}
}

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::OnRegister()
{
	Super::OnRegister();
	
	Bango::Debug::PrintComponentState(this, "OnRegister");
	
	if (Bango::Editor::IsComponentInEditedLevel(this))
	{
		FString ScriptName = GetName(); // We will use the component name for the script name
	
		FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &ScriptContainer, ScriptName);
	}
		
	if (!IsTemplate())
	{
		FBangoEditorDelegates::BangoDebugDraw.AddUObject(this, &ThisClass::DebugDraw);
	}
	
#if 0
	if (!Billboard && GetOwner() && !GetWorld()->IsGameWorld())
	{
		{
			FCookLoadScope EditorOnlyLoadScope(ECookLoadType::EditorOnly);
			const EObjectFlags TransactionalFlag = GetFlags() & RF_Transactional;
		
			Billboard = NewObject<UBillboardComponent>(GetOwner(), NAME_None, TransactionalFlag | RF_Transient | RF_TextExportTransient);
			Billboard->Sprite = IconTexture;
		}
		
		Billboard->SetupAttachment(GetOwner()->GetRootComponent());
		Billboard->bHiddenInGame = true;
		Billboard->bIsScreenSizeScaled = true;
		Billboard->SetRelativeLocation(100.0f * FVector::UpVector);
		
		
		Billboard->SetRelativeScale3D_Direct(FVector(1.0f, 1.0f, 1.0f));
		Billboard->Mobility = EComponentMobility::Movable;
		Billboard->AlwaysLoadOnClient = false;
		Billboard->SetIsVisualizationComponent(true);
		Billboard->SpriteInfo.Category = TEXT("Misc");
		Billboard->SpriteInfo.DisplayName = NSLOCTEXT("SpriteCategory", "Misc", "Misc");
		Billboard->CreationMethod = CreationMethod;
		Billboard->bIsScreenSizeScaled = true;
		Billboard->bUseInEditorScaling = true;
		Billboard->OpacityMaskRefVal = .3f;
		
		Billboard->RegisterComponent();
	}
#endif
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::OnUnregister()
{
	Bango::Debug::PrintComponentState(this, "OnUnregister_Early");
	
	FBangoEditorDelegates::BangoDebugDraw.RemoveAll(this);

	if (Bango::Editor::IsComponentInEditedLevel(this))
	{
		FBangoEditorDelegates::OnScriptContainerDestroyed.Broadcast(this, ScriptContainer.GetScriptClass());
	}
	
	if (GetOwner()->HasAnyFlags(RF_Transactional) && Bango::Editor::IsComponentInEditedLevel(this))
	{
		//FBangoEditorDelegates::OnScriptContainerUnregisteredDuringTransaction.Broadcast(this, ScriptContainer.GetScriptClass());
	}
	
	Super::OnUnregister();

	Bango::Debug::PrintComponentState(this, "OnUnregister_Finish");
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentCreated()
{
	Bango::Debug::PrintComponentState(this, "OnComponentCreated_Early");
	
	Super::OnComponentCreated();
	
	if (!Bango::Editor::IsComponentInEditedLevel(this))
	{
		return;
	}
	
	Bango::Debug::PrintComponentState(this, "OnComponentCreated_InLevel");
	
	// With RF_LoadCompleted this is a default actor component. We rely on PostDuplicated instead.
	if (HasAllFlags(RF_LoadCompleted))
	{
		return;
	}
	
	Bango::Debug::PrintComponentState(this, "OnComponentCreated_NotLoadCompleted");
	
	FString ScriptName = GetName(); // We will use the component name for the script name
	
	// TODO how do we handle copy-paste? PostEditImport?
	// If it already has a Guid, it was a duplicate or a copy paste, let PostDuplicate handle it for duplicate
	if (ScriptContainer.GetGuid().IsValid())
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &ScriptContainer, ScriptName);
	}
	else
	{
		FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &ScriptContainer, ScriptName);
	}
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	TSoftClassPtr<UBangoScript> ScriptClass = ScriptContainer.GetScriptClass();
	bool bIsComponentInEditedLevel = Bango::Editor::IsComponentInEditedLevel(this);
	
	Bango::Debug::PrintComponentState(this, "OnComponentDestroyed_Early");
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	
	Bango::Debug::PrintComponentState(this, "OnComponentDestroyed_PostSuper_0");
	
	if (!bIsComponentInEditedLevel)
	{
		return;
	}
	
	Bango::Debug::PrintComponentState(this, "OnComponentDestroyed_PostSuper_1");
	
	// This flag seems to be set when the editor destroys the component, e.g. it is unloaded by world partition. It isn't set when you delete the component. 	
	if (HasAllFlags(RF_BeginDestroyed))
	{
		return;
	}

	Bango::Debug::PrintComponentState(this, "OnComponentDestroyed_PostSuper_2");
	
	// If we are a default actor component, we will always exist on the actor and the only time we'll be truly deleted is when the whole actor hierachy is being deleted
	if (CreationMethod != EComponentCreationMethod::Instance && !bDestroyingHierarchy)
	{
		return;
	}

	Bango::Debug::PrintComponentState(this, "OnComponentDestroyed_PostSuper_Final");
	
	if (!ScriptClass.IsNull())
	{
		// Moves handling over to an editor module to handle more complicated package deletion/undo management
		//FBangoEditorDelegates::OnScriptContainerDestroyed.Broadcast(GetOwner(), ScriptClass);
	}
}

void UBangoScriptComponent::BeginDestroy()
{
	Bango::Debug::PrintComponentState(this, "BeginDestroy_Early");
	
	Super::BeginDestroy();
}

void UBangoScriptComponent::FinishDestroy()
{
	Bango::Debug::PrintComponentState(this, "FinishDestroy_Early");
	
	Super::FinishDestroy();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Bango::Debug::PrintComponentState(this, "PostDuplicate_Early");
	
	if (!Bango::Editor::IsComponentInEditedLevel(this))
	{
		return;
	}
	
	Bango::Debug::PrintComponentState(this, "PostDuplicate_InEditedLevel");
	
	if (CreationMethod == EComponentCreationMethod::Instance)
	{
		Bango::Debug::PrintComponentState(this, "PostDuplicate_Instance");
		
		// Component was added to an actor in the level; this case is very easy to handle, PostDuplicate is only called on real human-initiated duplications
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &ScriptContainer, GetName());
	}
	else
	{
		Bango::Debug::PrintComponentState(this, "PostDuplicate_CDO");

		// Component is part of a blueprint, only run duplication code if it's in a level already
		if (!GetOwner()->HasAnyFlags(RF_WasLoaded))
		{
			FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &ScriptContainer, GetName());
		}
	}
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::UnsetScript()
{
	Modify();
	ScriptContainer.Unset();
	
	if (!MarkPackageDirty())
	{
		UE_LOG(LogBlueprint, Error, TEXT("Could not mark the actor package dirty?"));
	}
}
#endif

// ----------------------------------------------

void UBangoScriptComponent::Run()
{
#if WITH_EDITOR
	RunningHandle = 
#endif
	UBangoScriptSubsystem::EnqueueScript(ScriptContainer.GetScriptClass(), GetOwner());
	
#if WITH_EDITOR
	if (RunningHandle.IsRunning())
	{
		TDelegate<void(FBangoScriptHandle)> OnFinished = TDelegate<void(FBangoScriptHandle)>::CreateUObject(this, &ThisClass::OnScriptFinished);
		UBangoScriptSubsystem::RegisterOnScriptFinished(this, RunningHandle, OnFinished);
	}
#endif
}

// ----------------------------------------------

#if WITH_EDITOR
FGuid UBangoScriptComponent::GetScriptGuid() const
{
	return ScriptContainer.GetGuid();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
UBangoScriptBlueprint* UBangoScriptComponent::GetScriptBlueprint(bool bForceLoad) const
{
	TSoftClassPtr<UBangoScript> ScriptClass = ScriptContainer.GetScriptClass();
	
	if (ScriptClass.IsNull())
	{
		return nullptr;
	}
	
	if (ScriptClass.IsValid())
	{
		TSubclassOf<UBangoScript> LoadedScriptClass = ScriptClass.Get();
		return Cast<UBangoScriptBlueprint>(UBlueprint::GetBlueprintFromClass(LoadedScriptClass));
	}
	
	if (ScriptClass.IsPending() && bForceLoad)
	{
		TSubclassOf<UBangoScript> LoadedScriptClass = ScriptClass.Get();
		return Cast<UBangoScriptBlueprint>(UBlueprint::GetBlueprintFromClass(LoadedScriptClass));
	}

	return nullptr;
}
#endif

// ----------------------------------------------

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
	
	ScriptContainer.SetScriptClass(Cast<UClass>(Blueprint->GeneratedClass));
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::OnScriptFinished(FBangoScriptHandle FinishedHandle)
{
	if (FinishedHandle != RunningHandle)
	{
		return;
	}
	
	RunningHandle.Expire();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::DebugDraw(FBangoDebugDrawCanvas& Canvas, bool bPIE) const
{
	if (bPIE)
	{
		DebugDrawGame(Canvas);
	}
	else
	{
		DebugDrawEditor(Canvas);
	}
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::DebugDrawEditor(FBangoDebugDrawCanvas& Canvas) const
{
	FLinearColor TagColor = Bango::Colors::White;

	float Alpha = Canvas.GetAlpha(GetOwner());
	
	if (Alpha <= 0.0f)
	{
		return;
	}
	
	FVector ScreenLocation = Canvas.GetNextScreenPos(GetOwner());
	
	if (ScreenLocation.Z <= 0.0f)
	{
		return;
	}
	
	if (GetWorld()->IsGameWorld())
	{
		if (RunningHandle.IsExpired())
		{
			TagColor = Bango::Colors::YellowBase;
		}
		else if (RunningHandle.IsRunning())
		{
			TagColor = Bango::Colors::LightBlue;
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
			TagColor = Bango::Colors::Green;
		}
	}
	
	FText LabelText = FText::FromString(ScriptContainer.GetDescription());
	
	if (LabelText.IsEmpty())
	{
		LabelText = FText::FromString(GetName());
	}
	
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
		// Background
		float X = ScreenLocation.X - 0.5f * TotalWidth - Padding;
		float Y = ScreenLocation.Y - 0.5f * TotalHeight - Padding;
		float XL = TotalWidth + 2.0f * Padding;
		float YL = TotalHeight + 2.0f * Padding;
		
		FVector4f UV(0.0f, 0.0f, 1.0f, 1.0f);
	
		UTexture* BackgroundTex = LoadObject<UTexture>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture"));
		
		// Larger whitish rectangle
		Canvas->SetDrawColor(FColor(150, 150, 150, 150 * Alpha));
		Canvas->DrawTile(BackgroundTex, X - Border, Y - Border, XL + 2.0 * Border, YL + 2.0 * Border, UV.X, UV.Y, UV.Z, UV.Z);
		
		// Darker background
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
		
		FCanvasTextItem Text(FVector2D(X + 0.5f * IconSize + IconPadding, Y), LabelText, Font, Alpha * Bango::Colors::White);
		Text.bCentreY = true;
		Canvas->DrawItem(Text);
	}
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::DebugDrawGame(FBangoDebugDrawCanvas& Canvas) const
{
	DebugDrawEditor(Canvas);
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::PreEditUndo()
{
	Super::PreEditUndo();
	
	Bango::Debug::PrintComponentState(this, "PreEditUndo");
	
	__UNDO_Script = ScriptContainer;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptComponent::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation)
{
	Super::Super::PostEditUndo(TransactionAnnotation);
	
	Bango::Debug::PrintComponentState(this, "PostEditUndo");
}
#endif

// ----------------------------------------------

#undef LOCTEXT_NAMESPACE