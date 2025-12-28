#include "BangoEditor/Menus/BangoEditorMenus.h"

#include "LevelEditor.h"
#include "SEditorViewport.h"
#include "Bango/Components/BangoActorIDComponent.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/Commands/BangoEditorActions.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Widgets/Input/STextEntryPopup.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

TSharedPtr<SWindow> FBangoEditorMenus::ActiveWindow;

FBangoEditorMenus::FBangoEditorMenus()
{
}

void FBangoEditorMenus::BindCommands()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		
	TSharedPtr<FUICommandList> GlobalCommands = LevelEditorModule.GetGlobalLevelEditorActions();
	
	GlobalCommands->MapAction(FBangoEditorCommands::Get().SetEditActorID, 
		FExecuteAction::CreateStatic(&FBangoEditorMenus::SetEditActorID),
		FCanExecuteAction::CreateStatic(&FBangoEditorMenus::Can_SetEditActorID));
}

void FBangoEditorMenus::BuildMenus()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([] ()
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.ActorContextMenu");
		
		Menu->AddDynamicSection("Bango", FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
		{
			FToolMenuSection& Section = InMenu->AddSection("Bango", INVTEXT("Bango"));
			
			Section.AddMenuEntry(FBangoEditorCommands::Get().SetEditActorID, LOCTEXT("SetEditActorID_MenuEntryText", "Set/Edit Actor ID"), LOCTEXT("SetActorID_Description", "Adds a Bango Actor ID Component onto the actor and sets the specified ID"), FSlateIcon("BangoEditorStyleSet", "Icon.Plunger"));
		}));
	}));
}

void FBangoEditorMenus::SetEditActorID()
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

	SetEditActorID(Actor);
}

void FBangoEditorMenus::SetEditActorID(AActor* Actor, bool bBlocking)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogBango, Error, TEXT("Tried to call SetEditActorID on invalid actor!"));
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
	//UE_LOG(LogTemp, Display, TEXT("%f, %f"), ScreenPos.X, ScreenPos.Y);
		
	if (SceneView)
	{
		FVector2D PixelPoint;
		SceneView->WorldToPixel(Actor->GetActorLocation() + 100 * FVector::UpVector, PixelPoint);
		ScreenPos.X = VPWindowPos.X + PixelPoint.X - 100;
		ScreenPos.Y = VPWindowPos.Y + PixelPoint.Y - 100;
		//UE_LOG(LogTemp, Display, TEXT("%f, %f"), ScreenPos.X, ScreenPos.Y);
	}

	UBangoActorIDComponent* ExistingIDComponent = Actor->FindComponentByClass<UBangoActorIDComponent>();
	FName ExistingName = NAME_None;
		
	if (IsValid(ExistingIDComponent))
	{
		ExistingName = ExistingIDComponent->GetBangoName();
	}
		
	TWeakObjectPtr<UBangoActorIDComponent> WeakExistingIDComponent = ExistingIDComponent;
	TWeakObjectPtr<AActor> WeakActor = Actor;
	
	TSharedPtr<SEditableTextBox> InputBox = CreateInputBox(WeakActor, WeakExistingIDComponent);
	TSharedPtr<SWidget> PopupWidget = GetIDEditingWidget(InputBox);
	PopupWidget->SlatePrepass(1.0);
	
	if (!bBlocking)
	{
		FSlateApplication::Get().PushMenu(
			FSlateApplication::Get().GetUserFocusedWidget(0).ToSharedRef(),
			FWidgetPath(),
			PopupWidget.ToSharedRef(),
			FDeprecateSlateVector2D(ScreenPos.X, ScreenPos.Y),
			FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup), 
			false
		);
	
		FSlateApplication::Get().SetKeyboardFocus(InputBox);
	}
	else
	{
		InputBox->SetText(FText::FromString(Actor->GetActorLabel()));
		
		ActiveWindow = SNew(SWindow)
			.Title(INVTEXT("Add ID"))
			.ClientSize(FVector2D(400, 150))
			.IsTopmostWindow(true)
			[
				PopupWidget.ToSharedRef()
			];
		
		FSlateApplication::Get().SetKeyboardFocus(InputBox.ToSharedRef());
		
		ActiveWindow->SetWidgetToFocusOnActivate(InputBox);
		
		FSlateApplication::Get().AddModalWindow(
			ActiveWindow.ToSharedRef(),
			FSlateApplication::Get().GetActiveTopLevelWindow()
			);
	}
}

bool FBangoEditorMenus::Can_SetEditActorID()
{
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	TArray<AActor*> SelectedActors;
		
	if (EditorActorSubsystem)
	{
		SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
			
		if (SelectedActors.Num() != 1)
		{
			return false;
		}
	}

	return true;
}

TSharedRef<SWidget> FBangoEditorMenus::GetIDEditingWidget(TSharedPtr<SEditableTextBox> InputBox)
{
	return SNew(SBorder)
	.Padding(8)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GetTitleWidget()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			//.AutoWidth()
			[
				SNew(SBox)
				.MinDesiredWidth(200)
				.ToolTipText(LOCTEXT("SetActorID", "Adds a Bango Actor ID Component onto the actor and sets the specified ID"))
				[
					InputBox.ToSharedRef()
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4, 12, 4, 4)
		[
			SNew(SBox)
			.MaxDesiredWidth(200)
			[
				SNew(STextBlock)
				.WrappingPolicy(ETextWrappingPolicy::DefaultWrapping)
				.AutoWrapText(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Normal", 8))
				.Text(LOCTEXT("SetActorIDWidget_FooterDescription", "This will add an ID component to the actor.\n\nSetting it blank will remove an existing component."))
			]
		]
	];
}

TSharedRef<SWidget> FBangoEditorMenus::GetTitleWidget()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.Padding(4, 0, 0, 4)
	.VAlign(VAlign_Bottom)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("EditActorIDWidget_Title", "Set Actor ID:"))
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.VAlign(VAlign_Top)
	[
		SNew(SButton)
		.Visibility_Lambda([]()
		{
			return FBangoEditorMenus::ActiveWindow.IsValid() ? EVisibility::Collapsed : EVisibility::Visible; 
		})
		.ToolTipText(LOCTEXT("EditActorIDWidget_CancelButtonToolTip", "Cancel"))
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ContentPadding(4)
		.OnClicked_Lambda( [] ()
		{
			FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
			return FReply::Handled();
		})
		[
			SNew(SImage)
			.DesiredSizeOverride(FVector2D(8, 8))
			.Image(FAppStyle::Get().GetBrush("Icons.X"))
		]
	];
}

UBangoActorIDComponent* FBangoEditorMenus::CreateIDComponent(TWeakObjectPtr<AActor> WeakActor)
{
	if (TStrongObjectPtr<AActor> Actor = WeakActor.Pin())
	{				
		static const FName ComponentName(TEXT("BangoActorID"));
		FName FinalName = MakeUniqueObjectName(Actor.Get(), UBangoActorIDComponent::StaticClass(), ComponentName, EUniqueObjectNameOptions::None);
		UBangoActorIDComponent* NewIDComponent = NewObject<UBangoActorIDComponent>(Actor.Get(), FinalName);
				
		if (IsValid(NewIDComponent))
		{
			Actor->Modify();
			Actor->AddInstanceComponent(NewIDComponent);
			NewIDComponent->RegisterComponent();

			FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
			LevelEditor.BroadcastComponentsEdited();
			
			return NewIDComponent;
		}
	}
	
	return nullptr;
}

void FBangoEditorMenus::DestroyIDComponent(TWeakObjectPtr<AActor> WeakActor, TWeakObjectPtr<UBangoActorIDComponent> WeakExistingIDComponent)
{
	if (TStrongObjectPtr<AActor> Actor = WeakActor.Pin())
	{
		if (WeakExistingIDComponent.IsValid())
		{
			FScopedTransaction T(LOCTEXT("RemoveActorID", "Remove Bango Actor ID"));
			
			Actor->Modify();
			Actor->RemoveInstanceComponent(WeakExistingIDComponent.Get());
			WeakExistingIDComponent->DestroyComponent();
					
			FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
			LevelEditor.BroadcastComponentsEdited();
		}
	}
				
}

void FBangoEditorMenus::OnTextCommitted_IDField(const FText& InText, ETextCommit::Type InCommitType, TWeakObjectPtr<AActor> WeakActor, TWeakObjectPtr<UBangoActorIDComponent> WeakIDComponent)
{
	if (InCommitType != ETextCommit::OnEnter) return;
					
	TStrongObjectPtr<AActor> Actor = WeakActor.Pin();
	TStrongObjectPtr<UBangoActorIDComponent> ExistingIDComponent = WeakIDComponent.Pin();
					
	FString NameCandidate = InText.ToString();
	FName NewID;
	FText OutErrorText;
		
	if (NameCandidate.Len() == 0 && WeakIDComponent.IsValid())
	{
		FScopedTransaction T(LOCTEXT("RemoveActorID", "Remove Bango Actor ID"));
		Actor->Modify();
				
		Actor->RemoveInstanceComponent(ExistingIDComponent.Get());
		ExistingIDComponent->DestroyComponent();
				
		FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditor.BroadcastComponentsEdited();
		return;
	}
					
	if (FName::IsValidXName(NameCandidate, INVALID_NAME_CHARACTERS, &OutErrorText))
	{
		NewID = FName(NameCandidate);
	}
	else
	{
		return;
	}
		
	if (ExistingIDComponent.IsValid())
	{
		FScopedTransaction T(LOCTEXT("EditActorID", "Edit Bango Actor ID")); // lol this doesn't work
		ExistingIDComponent->SetActorID(FName(NewID));
	}
	else
	{
		FScopedTransaction T(LOCTEXT("SetNewActorID", "Set New Bango Actor ID")); // lol this doesn't work
		UBangoActorIDComponent* NewComponent = CreateIDComponent(WeakActor);
						
		if (NewComponent)
		{
			NewComponent->SetActorID(NewID);
		}
	}
	
	if (ActiveWindow.IsValid())
	{
		ActiveWindow->RequestDestroyWindow();
	}
}

TSharedPtr<SEditableTextBox> FBangoEditorMenus::CreateInputBox(TWeakObjectPtr<AActor> WeakActor, TWeakObjectPtr<UBangoActorIDComponent> WeakIDComponent)
{
	return SNew(SEditableTextBox)
	.HintText(INVTEXT("Enter ID..."))
	.ToolTipText(LOCTEXT("EditActorID_ToolTip", "Clear to remove ID"))
	.Text_Lambda([WeakIDComponent] ()
	{
		if (!WeakIDComponent.IsValid())
		{
			return FText::GetEmpty();
		}
		
		return FText::FromName(WeakIDComponent->GetBangoName());
	})
	.SelectAllTextWhenFocused(true)
	.ClearKeyboardFocusOnCommit(true)
	.OnTextCommitted_Static(&FBangoEditorMenus::OnTextCommitted_IDField, WeakActor, WeakIDComponent);
}

#undef LOCTEXT_NAMESPACE
