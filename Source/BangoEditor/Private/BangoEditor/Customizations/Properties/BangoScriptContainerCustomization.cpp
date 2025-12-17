#include "BangoEditor/Customizations/Properties/BangoScriptContainerCustomization.h"

#include "BlueprintEditor.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "SEditorViewport.h"
#include "Bango/Core/BangoScript.h"
#include "BangoEditor/BangoColor.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Subsystems/BangoEditorSubsystem.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

// ----------------------------------------------

FBangoScriptContainerCustomization::FBangoScriptContainerCustomization()
{
	PostScriptCreated.AddRaw(this, &FBangoScriptContainerCustomization::OnPostScriptCreated);
	PreScriptDeleted.AddRaw(this, &FBangoScriptContainerCustomization::OnPreScriptDeleted);
	
	UBangoEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>();
	
	if (Subsystem)
	{
		Subsystem->OnScriptGenerated.AddRaw(this, &FBangoScriptContainerCustomization::OnPostScriptCreated);
	}
}

FBangoScriptContainerCustomization::~FBangoScriptContainerCustomization()
{
	UBangoEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>();
	
	if (Subsystem)
	{
		Subsystem->OnScriptGenerated.RemoveAll(this);
	}
}

// ----------------------------------------------

TSharedRef<IPropertyTypeCustomization> FBangoScriptContainerCustomization::MakeInstance()
{
	return MakeShared<FBangoScriptContainerCustomization>();
}

// ----------------------------------------------

UEdGraph* FBangoScriptContainerCustomization::GetPrimaryEventGraph() const
{
	UBlueprint* Blueprint = GetBlueprint();
	
	if (!Blueprint)
	{
		return nullptr;
	}
	
	if (Blueprint->UbergraphPages.Num() > 0)
	{
		return Blueprint->UbergraphPages[0];
	}

	return nullptr;
}

// ----------------------------------------------

void FBangoScriptContainerCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//ScriptBlueprintProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptBlueprint));
	ScriptClassProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass));
	GuidProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, Guid));
	
	CurrentGraph = GetPrimaryEventGraph();
	
	Box = SNew(SVerticalBox);
	
	HeaderRow.WholeRowContent()
	[
		SNew(SBox)
		.Padding(0, 4, 0, 4)
		[
			SNew(SBorder)
			.Padding(2)
			.BorderImage(FBangoEditorStyle::GetImageBrush(BangoEditorBrushes.Border_InlineBlueprintGraph))
			.BorderBackgroundColor(BangoColor::Noir)
			[
				Box.ToSharedRef()
			]			
		]
	];

	HeaderRow.ResetToDefaultContent()
	[
		SNullWidget::NullWidget
	];
	
	UpdateBox();	
	
	/*
	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("BangoScriptHolder_ScriptPropertyLabel", "Bango Script"))
		.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
	];
	
	HeaderRow.ValueContent()
	[
		SNew(SWidgetSwitcher)
		.WidgetIndex(this, &FBangoScriptContainerCustomization::WidgetIndex_CreateDeleteScriptButtons)
		+ SWidgetSwitcher::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("BangoScriptHolder_CreateScriptButtonText", "None (Click to Create)"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.OnClicked(this, &FBangoScriptContainerCustomization::OnClicked_CreateScript)
		]
		+ SWidgetSwitcher::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(LOCTEXT("BangoScriptHolder_DeleteScriptButtonText", "Delete"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
				.OnClicked(this, &FBangoScriptContainerCustomization::OnClicked_DeleteScript)
			]
		]
	];
	*/
}

// ----------------------------------------------

int FBangoScriptContainerCustomization::WidgetIndex_CreateDeleteScriptButtons() const
{
	if (IsValid(GetScriptClass()))
	{
		return 1;
	}
	
	return 0;
}

// ----------------------------------------------

void FBangoScriptContainerCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (GetOuter()->IsA(UBangoScriptComponent::StaticClass()))
	{
		return;
	}
	
	ChildBuilder.AddCustomRow(INVTEXT("ASDF"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("BangoScriptNameOverride_Label", "Script Name"))
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("BangoScriptNameOverride_HintText", "Optional"))	
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("BangoScriptNameOverride_ApplyButtonLabel", "Apply"))
			.OnClicked(this, &FBangoScriptContainerCustomization::OnClicked_RenameScript)
		]
	];
	return;
	/*
	Box = SNew(SVerticalBox);
	
	ChildBuilder.AddCustomRow(LOCTEXT("BangoScriptHolder_SearchTerm", "Bango"))
	.ShouldAutoExpand(true)
	[
		Box.ToSharedRef()
	];

	UpdateBox();
	*/
}

// ----------------------------------------------

int FBangoScriptContainerCustomization::WidgetIndex_GraphEditor() const
{
	return 0;
}

// ----------------------------------------------

FReply FBangoScriptContainerCustomization::OnClicked_CreateScript()
{
	TArray<UPackage*> Packages;
	
	ScriptClassProperty->GetOuterPackages(Packages);
	
	if (Packages.Num() != 1)
	{
		return FReply::Handled();
	}

	void* GuidPtr;
	GuidProperty->GetValueData(GuidPtr);
	FGuid* Guid = reinterpret_cast<FGuid*>(GuidPtr);
	
	FString BPName;
	UPackage* ScriptPackage = Bango::Editor::MakePackageForScript(ScriptClassProperty, Packages[0], BPName, *Guid);
	
	if (!ScriptPackage)
	{
		return FReply::Handled();
	}
	
	UBlueprint* Script = Bango::Editor::MakeScriptAsset(ScriptPackage, BPName, *Guid);
	
	if (!Script)
	{
		return FReply::Handled();
	}

	if (Bango::Editor::SaveScriptPackage(ScriptPackage, Script))
	{
		ScriptClassProperty->SetValue(Script->GeneratedClass);
		PostScriptCreated.Broadcast();
	}
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptContainerCustomization::OnClicked_DeleteScript()
{
	PreScriptDeleted.Broadcast();
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptContainerCustomization::OnClicked_EditScript() const
{
	UAssetEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

	if (Subsystem)
	{
		UBlueprint* ScriptBlueprint = UBlueprint::GetBlueprintFromClass(GetScriptClass());
		
		Subsystem->OpenEditorForAsset(ScriptBlueprint);
	}
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptContainerCustomization::OnClicked_EnlargeGraphView() const
{
	return OnClicked_EditScript();
	/*
	FViewport* Viewport = GEditor->GetActiveViewport();
	FViewportClient* ViewportClient = Viewport->GetClient();
	FEditorViewportClient* EditorViewportClient = static_cast<FEditorViewportClient*>(ViewportClient);
	FDeprecateSlateVector2D VPWindowPos = EditorViewportClient->GetEditorViewportWidget().Get()->GetCachedGeometry().GetAbsolutePosition();

	FVector2D ScreenPos(200, 200);

	TSharedRef<SWidget> Popout = GetPopoutGraphEditor();
	
	FSlateApplication::Get().PushMenu(
		FSlateApplication::Get().GetUserFocusedWidget(0).ToSharedRef(),
		FWidgetPath(),
		Popout,
		FDeprecateSlateVector2D(ScreenPos.X, ScreenPos.Y),
		FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup),
		true);
	
	return FReply::Handled();*/
}

FReply FBangoScriptContainerCustomization::OnClicked_RenameScript() const
{
	return FReply::Handled();
}

// ----------------------------------------------

TSharedRef<SWidget> FBangoScriptContainerCustomization::GetPopoutGraphEditor() const
{
	SGraphEditor::FGraphEditorEvents Events;
	
	FVector2D ParentWindowSize = FSlateApplication::Get().GetActiveTopLevelWindow()->GetSizeInScreen();
	
	const float CornerInset = 200.0f;
	
	ParentWindowSize.X -= 2.0f * CornerInset;
	ParentWindowSize.Y -= 2.0f * CornerInset;
	
	return SNew(SBorder)
		.Padding(20)
		[
			SNew(SBox)
			.WidthOverride(ParentWindowSize.X)
			.HeightOverride(ParentWindowSize.Y)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SGraphEditor)
					.GraphToEdit(GetPrimaryEventGraph())
					.IsEditable(false)
					.GraphEvents(Events)
					.ShowGraphStateOverlay(false)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Top)
				.Padding(20)
				[
					SNew(SButton)
					.OnClicked_Lambda([] ()
					{
						FSlateApplication::Get().ClearAllUserFocus();
						return FReply::Handled();
					})
					[
						SNew(SImage)
						.DesiredSizeOverride(FVector2D(24, 24))
						.Image(FAppStyle::GetBrush("Icons.X"))
					]
				]
			]
		];
}

void FBangoScriptContainerCustomization::OnPostScriptCreated()
{
	CurrentGraph = GetPrimaryEventGraph();
	
	UpdateBox();	
}

void FBangoScriptContainerCustomization::OnPreScriptDeleted()
{
	CurrentGraph = nullptr;
	
	UpdateBox();
}

void FBangoScriptContainerCustomization::UpdateBox()
{
	SGraphEditor::FGraphEditorEvents Events;
	
	Box->ClearChildren();
	
	if (CurrentGraph.IsValid())
	{
		Box->AddSlot()
		.AutoHeight()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.HeightOverride(300)
				[
					SNew(SGraphEditor)
					.GraphToEdit(GetPrimaryEventGraph())
					.IsEditable(false)
					.GraphEvents(Events)
					.ShowGraphStateOverlay(false)
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(8)
			[
				SNew(SButton)
				.OnClicked(this, &FBangoScriptContainerCustomization::OnClicked_EnlargeGraphView)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.Fullscreen"))
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Bottom)
			.Padding(8)
			[
				SNew(STextBlock)
				.Text(FText::Format
					(
						INVTEXT("{0}\n{1}{2}"),
						FText::FromName(GetBlueprint()->GetFName()),
						FText::FromString( GetBlueprint()->GetPackage()->GetPathName()),
						FText::FromString(FPackageName::GetAssetPackageExtension())
					))
				.Font(FCoreStyle::GetDefaultFontStyle("Normal", 8))
				.ColorAndOpacity(BangoColor::Gray)
			]
		];
	}
	else
	{
		Box->AddSlot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("BangoScriptHolder_NoScriptGraphLabel", "No script"))
		];
	}
}

// ----------------------------------------------

AActor* FBangoScriptContainerCustomization::GetOwner() const
{
	TArray<UObject*> OuterObjects;
	ScriptClassProperty->GetOuterObjects(OuterObjects);
	
	if (OuterObjects.Num() > 0)
	{
		if (AActor* Actor = Cast<AActor>(OuterObjects[0]))
		{
			return Actor;
		}
		else if (UActorComponent* Component = Cast<UActorComponent>(OuterObjects[0]))
		{
			return Component->GetOwner();
		}
	}
	
	return nullptr;
}

UObject* FBangoScriptContainerCustomization::GetOuter() const
{
	TArray<UObject*> OuterObjects;
	ScriptClassProperty->GetOuterObjects(OuterObjects);
	
	if (OuterObjects.Num() == 1)
	{
		return OuterObjects[0];
	}
	
	return nullptr;
}

// ----------------------------------------------

UBlueprint* FBangoScriptContainerCustomization::GetBlueprint() const
{
	UObject* ScriptClassObject;
	
	if (ScriptClassProperty->GetValue(ScriptClassObject))
	{
		if (UClass* Class = Cast<UClass>(ScriptClassObject))
		{
			if (UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(Class))
			{
				return Blueprint;
			}
		}
	}
	
	return nullptr;
}

// ----------------------------------------------

TSubclassOf<UBangoScript> FBangoScriptContainerCustomization::GetScriptClass() const
{
	UObject* ClassObject;
	
	if (ScriptClassProperty->GetValue(ClassObject) == FPropertyAccess::Success)
	{
		return Cast<UClass>(ClassObject);
	}

	return nullptr;
}

// ----------------------------------------------

// ----------------------------------------------

#undef LOCTEXT_NAMESPACE