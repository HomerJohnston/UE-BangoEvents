#include "BangoEditor/Customizations/Properties/BangoScriptContainerCustomization.h"

#include "BlueprintEditor.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "SEditorViewport.h"
#include "Bango/Core/BangoScript.h"
#include "BangoEditorTooling/BangoColors.h"
#include "BangoEditorTooling/BangoEditorDelegates.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoEditor/Subsystems/BangoEditorSubsystem.h"
#include "BangoEditor/BlueprintEditor/BangoBlueprintEditor.h"
#include "BangoEditor/Widgets/SBangoGraphEditor.h"
#include "EdGraph/EdGraph.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

class FWidgetBlueprintEditor;

// ==============================================

FBangoScriptContainerCustomization::FBangoScriptContainerCustomization()
{
	PostScriptCreated.AddRaw(this, &FBangoScriptContainerCustomization::OnPostScriptCreatedOrRenamed);
	PreScriptDeleted.AddRaw(this, &FBangoScriptContainerCustomization::OnPreScriptDeleted);
	
	UBangoLevelScriptsEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoLevelScriptsEditorSubsystem>();
	
	if (Subsystem)
	{
		Subsystem->OnScriptGenerated.AddRaw(this, &FBangoScriptContainerCustomization::OnPostScriptCreatedOrRenamed);
	}
}

// ----------------------------------------------

FBangoScriptContainerCustomization::~FBangoScriptContainerCustomization()
{
	UBangoLevelScriptsEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoLevelScriptsEditorSubsystem>();
	
	if (Subsystem)
	{
		Subsystem->OnScriptGenerated.RemoveAll(this);
	}
	
	FWorldDelegates::OnWorldBeginTearDown.RemoveAll(this);
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
	if (PropertyHandle->GetNumOuterObjects() != 1)
	{
		HeaderRow.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("MultipleScriptsEdited_Warning", "    <Multiple Scripts>"))
			.TextStyle(FAppStyle::Get(), "SmallText")
		];
		
		return;
	}
	
	ScriptContainerProperty = PropertyHandle;
	ScriptClassProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass));
		
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
			.BorderBackgroundColor(Bango::Colors::Noir)
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
	
	UBlueprint* Blueprint = GetBlueprint();
	
	if (Blueprint)
	{
		SetProposedScriptName(FText::FromString(Blueprint->GetName()));
	}
	
	// We kill the graph widgets immediately to avoid garbage collection and file access issues
	FEditorDelegates::OnMapLoad.AddSP(this, &FBangoScriptContainerCustomization::OnMapLoad);
	FBangoEditorDelegates::OnScriptContainerDestroyed.AddSP(this, &FBangoScriptContainerCustomization::OnScriptContainerDestroyed);
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
	if (PropertyHandle->GetNumOuterObjects() != 1)
	{
		return;
	}
	
	UObject* Outer = GetOuter();
	
	uint32 NumChildren;
	PropertyHandle->GetNumChildren(NumChildren);
	
	for (uint32 ChildIndex = 0; ChildIndex <  NumChildren; ++ChildIndex)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = PropertyHandle->GetChildHandle(ChildIndex);
		
		if (ChildHandle->IsValidHandle() && ChildHandle->IsEditable())
		{
			ChildBuilder.AddProperty(ChildHandle.ToSharedRef());
		}
	}
	
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
	
	void* ScriptContainerPtr = nullptr;
	ScriptContainerProperty->GetValueData(ScriptContainerPtr);
	
	FBangoScriptContainer* ScriptContainer = reinterpret_cast<FBangoScriptContainer*>(ScriptContainerPtr);
	
	UObject* Outer = GetOuter();
	
	//FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(Outer, ScriptContainer, *Outer->GetFName().ToString());
	
	/*
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

	//if (Bango::Editor::SaveScriptPackage(ScriptPackage, Script))
	//{
		ScriptClassProperty->SetValue(Script->GeneratedClass);
		PostScriptCreated.Broadcast();
	//}
	*/
	
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
		UBlueprint* ScriptBlueprint = GetBlueprint();
		
		if (ScriptBlueprint)
		{
			Subsystem->OpenEditorForAsset(ScriptBlueprint);
		}
	}
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptContainerCustomization::OnClicked_EnlargeGraphView() const
{
#if 1
	TSharedRef<FBangoBlueprintEditor> NewBlueprintEditor(new FBangoBlueprintEditor());

	const bool bShouldOpenInDefaultsMode = false;
	TArray<UBlueprint*> Blueprints;
	Blueprints.Add(GetBlueprint());

	NewBlueprintEditor->InitBlueprintEditor(EToolkitMode::Standalone, nullptr, Blueprints, bShouldOpenInDefaultsMode);
	//NewBlueprintEditor->InitWidgetBlueprintEditor(EToolkitMode::Standalone, nullptr, Blueprints, bShouldOpenInDefaultsMode);
	
	return FReply::Handled();
#endif 

#if 0
	FViewport* Viewport = GEditor->GetActiveViewport();
	FViewportClient* ViewportClient = Viewport->GetClient();
	FEditorViewportClient* EditorViewportClient = static_cast<FEditorViewportClient*>(ViewportClient);
	FDeprecateSlateVector2D VPWindowPos = EditorViewportClient->GetEditorViewportWidget().Get()->GetCachedGeometry().GetAbsolutePosition();

	TSharedPtr<SWindow> UEWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	
	FVector2D WindowPos(0);
	FVector2D WindowSize(400);
	
	if (UEWindow.IsValid())
	{
		FVector2D MainWindowSize = UEWindow->GetClientSizeInScreen();
		FVector2D MainWindowPos = UEWindow->GetPositionInScreen();

		float CornerInset = FMath::RoundToFloat( 0.1f * MainWindowSize.Y );

		WindowPos = FVector2D(MainWindowPos.X + CornerInset, MainWindowPos.Y + CornerInset);
		WindowSize = FVector2D(MainWindowSize.X - 2.0f * CornerInset, MainWindowSize.Y - 2.0f * CornerInset);
	}

	TSharedRef<SWidget> Popout = GetPopoutGraphEditor(WindowSize);
	
	FSlateApplication::Get().PushMenu(
		FSlateApplication::Get().GetUserFocusedWidget(0).ToSharedRef(),
		FWidgetPath(),
		Popout,
		FDeprecateSlateVector2D(WindowPos.X, WindowPos.Y),
		FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup),
		true);
	
	return FReply::Handled();
#endif
}

FReply FBangoScriptContainerCustomization::OnClicked_RenameScript() const
{
	return FReply::Handled();
}

void FBangoScriptContainerCustomization::OnTextChanged_ScriptNameEditableText(const FText& Text)
{
	SetProposedScriptName(Text);
}

void FBangoScriptContainerCustomization::SetProposedScriptName(const FText& Text)
{
	ScriptNameText = Text;
	ProposedNameStatus = GetProposedNameStatus();
}

EBangoScriptRenameStatus FBangoScriptContainerCustomization::GetProposedNameStatus()
{
	UPackage* Package = GetBlueprint()->GetPackage();
	
	if (Package)
	{
		TArray<UObject*> Objects;
		GetObjectsWithPackage(Package, Objects, false);
		
		FString ProposedBlueprintClassName = ScriptNameText.ToString();
		
		if (ProposedBlueprintClassName == GetBlueprint()->GetName())
		{
			return EBangoScriptRenameStatus::MatchesCurrent;
		}
		
		if (!ProposedBlueprintClassName.EndsWith(TEXT("_C")))
		{
			ProposedBlueprintClassName.Append(TEXT("_C"));
		}
		
		bool bMatch = false;
		
		for (UObject* Object : Objects)
		{
			// I need to make sure we don't cause *any* object name conflicts
			/*
			if (!Object->IsA<UBlueprintGeneratedClass>())
			{
				continue;
			}
			*/
			
			if (ProposedBlueprintClassName == Object->GetName())
			{
				bMatch = true;
				break;
			}
		}
				
		if (bMatch)
		{
			return EBangoScriptRenameStatus::MatchesOther;
		}

		// TODO check for valid chars
		return EBangoScriptRenameStatus::ValidNewName;
	}

	return EBangoScriptRenameStatus::InvalidNewName;
}

FSlateColor FBangoScriptContainerCustomization::ForegroundColor_ScriptNameEditableText() const
{
	return FocusedForegroundColor_ScriptNameEditableText().GetSpecifiedColor().Desaturate(0.25f);
}

FSlateColor FBangoScriptContainerCustomization::FocusedForegroundColor_ScriptNameEditableText() const
{
	switch (ProposedNameStatus)
	{
		case EBangoScriptRenameStatus::MatchesCurrent:
		{
			return Bango::Colors::Gray;
		}
		case EBangoScriptRenameStatus::MatchesOther:
		{
			return Bango::Colors::LightRed;
		}
		case EBangoScriptRenameStatus::ValidNewName:
		{
			return Bango::Colors::LightGreen;
		}
		default:
		{
			return Bango::Colors::Error;
		}
	}
}

bool FBangoScriptContainerCustomization::IsEnabled_RenameScriptButton() const
{
	switch (ProposedNameStatus)
	{
		case EBangoScriptRenameStatus::ValidNewName:
		{
			return true;
		}
		default:
		{
			return false;
		}
	}
}

// ----------------------------------------------

TSharedRef<SWidget> FBangoScriptContainerCustomization::GetPopoutGraphEditor(FVector2D WindowSize) const
{
	SGraphEditor::FGraphEditorEvents Events;
	
	const bool bShouldOpenInDefaultsMode = false;
	TSharedRef<FBangoBlueprintEditor> BlueprintEditor = MakeShared<FBangoBlueprintEditor>();
	
	// Not working. 
	BlueprintEditor->InitBlueprintEditor(EToolkitMode::Standalone, nullptr, { GetBlueprint() }, bShouldOpenInDefaultsMode);
	// BlueprintEditor->SetupGraphEditorEvents_Impl(GetBlueprint(), GetPrimaryEventGraph(), Events);
	
	return SNew(SBorder)
	.Padding(20)
	[
		SNew(SBox)
		.WidthOverride(WindowSize.X)
		.HeightOverride(WindowSize.Y)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBangoGraphEditor)
				.BlueprintEditor(BlueprintEditor)
				.GraphToEdit(GetPrimaryEventGraph())
				.IsEditable(false) // (true) // Not working
				.GraphEvents(Events)
				.ShowGraphStateOverlay(true)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(20)
			[
				SNew(SButton)
				.OnClicked_Lambda([this] ()
				{
					UAssetEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

					if (Subsystem)
					{
						UBlueprint* ScriptBlueprint = GetBlueprint();
						
						if (ScriptBlueprint)
						{
							Subsystem->OpenEditorForAsset(ScriptBlueprint);
						}
					}
	
					return FReply::Handled();
				})
				[
					SNew(SImage)
					.DesiredSizeOverride(FVector2D(24, 24))
					.Image(FAppStyle::GetBrush("Sequencer.AllowAllEdits")) // TODO custom icon
				]
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

void FBangoScriptContainerCustomization::OnPostScriptCreatedOrRenamed()
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
		TSharedRef<SGraphEditor> GraphEditor = SNew(SGraphEditor)
		.GraphToEdit(GetPrimaryEventGraph())
		.IsEditable(false)
		.GraphEvents(Events)
		.ShowGraphStateOverlay(false);
		
		Box->AddSlot()
		.AutoHeight()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.HeightOverride(300)
				[
					GraphEditor
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
						FText::FromString( GetBlueprint()->GetPackage()->ContainsMap() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension())
					))
				.Font(FCoreStyle::GetDefaultFontStyle("Normal", 8))
				.ColorAndOpacity(Bango::Colors::Gray)
			]
		];
	}
	else
	{
		Box->AddSlot()
		[
			SNew(SButton)
			.Text(LOCTEXT("BangoScriptHolder_CreateScriptButtonText", "None (Click to Create)"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.OnClicked(this, &FBangoScriptContainerCustomization::OnClicked_CreateScript)
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
	void* ScriptClassPtr = nullptr;
	
	if (ScriptClassProperty->GetValueData(ScriptClassPtr) == FPropertyAccess::Result::Success)
	{
		TSoftClassPtr<UBangoScript>* ScriptClass = reinterpret_cast<TSoftClassPtr<UBangoScript>*>(ScriptClassPtr);

		if (ScriptClass)
		{
			UObject* ScriptClassLoaded = ScriptClass->LoadSynchronous();
			
			TSubclassOf<UBangoScript> BangoScriptClassLoaded = Cast<UClass>(ScriptClassLoaded);
			
			if (UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(BangoScriptClassLoaded))
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

void FBangoScriptContainerCustomization::OnScriptContainerDestroyed(UObject* Object, FBangoScriptContainer* ScriptContainer)
{
	OnPreScriptDeleted();
}

void FBangoScriptContainerCustomization::OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap)
{
	// If we don't get rid of the blueprint graph, we get an editor crash when switching maps.
	Box->ClearChildren();
	CurrentGraph = nullptr;
}

// ----------------------------------------------

// ----------------------------------------------

#undef LOCTEXT_NAMESPACE