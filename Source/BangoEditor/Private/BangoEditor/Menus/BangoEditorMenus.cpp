#include "BangoEditor/Menus/BangoEditorMenus.h"

#include "LevelEditor.h"
#include "SEditorViewport.h"
#include "Bango/Components/BangoActorIDComponent.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Widgets/Input/STextEntryPopup.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

void FBangoEditorMenus::RegisterMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.ActorContextMenu");

	bool BB = Menu->ContainsSection("ActorUETools");
	bool CC = Menu->ContainsSection("ActorTypeTools");
	
	//FToolMenuSection& Section = Menu->FindOrAddSection("ActorUETools"); // ActorTypeTools doesn't work it's the Preview category???????? Neither does ActorUETools????????
	FToolMenuSection& Section = Menu->FindOrAddSection("ActorOptions");
	
	FToolMenuEntry& Entry = Section.AddDynamicEntry("BangoSetActorName", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		TArray<AActor*> SelectedActors;
		
		if (EditorActorSubsystem)
		{
			SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
			
			if (SelectedActors.Num() != 1)
			{
				return;
			}
		}
		else
		{
			return;
		}

		AActor* Actor = SelectedActors[0];

		if (!IsValid(Actor))
		{
			return;
		}

		FViewport* Viewport = GEditor->GetActiveViewport();
		
		FViewportClient* ViewportClient = Viewport->GetClient();

		FEditorViewportClient* EditorViewportClient = static_cast<FEditorViewportClient*>(ViewportClient);

		FDeprecateSlateVector2D VPWindowPos = EditorViewportClient->GetEditorViewportWidget().Get()->GetCachedGeometry().GetAbsolutePosition();

		FSceneViewFamilyContext ViewFamilyContext(FSceneViewFamily::ConstructionValues(
			EditorViewportClient->Viewport,
			EditorViewportClient->GetWorld()->Scene,
			EditorViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));
		
		FSceneView* SceneView = EditorViewportClient->CalcSceneView(&ViewFamilyContext);
		
		FVector2D ScreenPos = FSlateApplication::Get().GetCursorPos();
		UE_LOG(LogTemp, Display, TEXT("%f, %f"), ScreenPos.X, ScreenPos.Y);
		
		if (SceneView)
		{
			FVector2D PixelPoint;
			SceneView->WorldToPixel(Actor->GetActorLocation(), PixelPoint);
			ScreenPos.X = VPWindowPos.X + PixelPoint.X;
			ScreenPos.Y = VPWindowPos.Y + PixelPoint.Y;
			UE_LOG(LogTemp, Display, TEXT("%f, %f"), ScreenPos.X, ScreenPos.Y);
		}

		UBangoActorIDComponent* ExistingIDComponent = Actor->FindComponentByClass<UBangoActorIDComponent>();
		FName ExistingName = NAME_None;
		
		if (IsValid(ExistingIDComponent))
		{
			ExistingName = ExistingIDComponent->GetActorID();
		}
		
		FToolUIActionChoice Action(FExecuteAction::CreateLambda([ScreenPos, ExistingName, Actor, ExistingIDComponent]()
		{
			TSharedRef<STextEntryPopup> TextEntry = 
			SNew(STextEntryPopup)
				.Label(LOCTEXT("SetActorIDContextMenuEntry", "Set Actor ID"))
				.DefaultText(FText::FromName(ExistingName))
				.OnTextCommitted_Lambda([Actor, ExistingIDComponent] (const FText& InText, ETextCommit::Type InCommitType)
				{
					if (InCommitType != ETextCommit::OnEnter) return;
					
					FString NameCandidate = InText.ToString();
					FName NewID;
					FText OutErrorText;
					
					if (FName::IsValidXName(NameCandidate, INVALID_NAME_CHARACTERS, &OutErrorText))
					{
						NewID = FName(NameCandidate);
					}
					else
					{
						return;
					}
					
					if (ExistingIDComponent)
					{
						FScopedTransaction T(LOCTEXT("EditActorID", "Edit Actor ID")); // lol this doesn't work
						ExistingIDComponent->SetActorID(FName(NewID));
					}
					else
					{
						FScopedTransaction T(LOCTEXT("SetNewActorID", "Set New Actor ID")); // lol this doesn't work
						
						static const FName ComponentName(TEXT("BangoActorID"));
						FName FinalName = MakeUniqueObjectName(Actor, UBangoActorIDComponent::StaticClass(), ComponentName, EUniqueObjectNameOptions::None);
						UBangoActorIDComponent* NewIDComponent = NewObject<UBangoActorIDComponent>(Actor, FinalName);
						
						if (IsValid(NewIDComponent))
						{
							NewIDComponent->SetActorID(NewID);
							
							Actor->Modify();
							Actor->AddInstanceComponent(NewIDComponent);
							NewIDComponent->RegisterComponent();

							FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
							LevelEditor.BroadcastComponentsEdited();
						}
					}
				})
				.SelectAllTextWhenFocused(true)
				.ClearKeyboardFocusOnCommit(true);

			FSlateApplication::Get().PushMenu(
				FSlateApplication::Get().GetUserFocusedWidget(0).ToSharedRef(),
				FWidgetPath(),
				TextEntry,
				FDeprecateSlateVector2D(ScreenPos.X, ScreenPos.Y),
				FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
				);
		}));

		InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("BangoSetActorID"), INVTEXT("Set Actor ID"), INVTEXT("Description!"), FSlateIcon(FName(""), ""), Action));
	}));
}

#undef LOCTEXT_NAMESPACE