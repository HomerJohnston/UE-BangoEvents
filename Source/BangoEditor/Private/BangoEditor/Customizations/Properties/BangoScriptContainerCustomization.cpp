#include "BangoEditor/Customizations/Properties/BangoScriptContainerCustomization.h"

#include "BlueprintEditor.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "SEditorViewport.h"
#include "WidgetBlueprintEditor.h"
#include "Bango/Core/BangoScript.h"
#include "BangoEditor/BangoColor.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "BangoEditor/Subsystems/BangoEditorSubsystem.h"
#include "BangoEditor/Utilities/BangoEditorUtility.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/DefinePrivateMemberPtr.h"
#include "UObject/SavePackage.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "Bango/Private/Bango/ThirdParty/BMPrivateAccess.h"
#include "Bango/Utility/BangoHelpers.h"
#include "BangoEditor/BlueprintEditor/BangoBlueprintEditor.h"
#include "BangoEditor/Widgets/SBangoGraphEditor.h"
#include "Editor/UMGEditor/Public/WidgetBlueprintEditor.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

class FWidgetBlueprintEditor;

// ==============================================

FBangoScriptContainerCustomization::FBangoScriptContainerCustomization()
{
	PostScriptCreated.AddRaw(this, &FBangoScriptContainerCustomization::OnPostScriptCreatedOrRenamed);
	PreScriptDeleted.AddRaw(this, &FBangoScriptContainerCustomization::OnPreScriptDeleted);
	
	UBangoEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>();
	
	if (Subsystem)
	{
		Subsystem->OnScriptGenerated.AddRaw(this, &FBangoScriptContainerCustomization::OnPostScriptCreatedOrRenamed);
	}
}

// ----------------------------------------------

FBangoScriptContainerCustomization::~FBangoScriptContainerCustomization()
{
	UBangoEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UBangoEditorSubsystem>();
	
	if (Subsystem)
	{
		Subsystem->OnScriptGenerated.RemoveAll(this);
	}
	
	UBangoScript::SelectedScript.Reset();
	
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
		HeaderRow.WholeRowContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("MultipleScriptsEdited_Warning", "    <Multiple Scripts>"))
			.TextStyle(FAppStyle::Get(), "SmallText")
		];
		
		return;
	}
	
	//ScriptBlueprintProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptBlueprint));
	ScriptContainerProperty = PropertyHandle;
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
	
	UBlueprint* Blueprint = GetBlueprint();
	
	if (Blueprint)
	{
		SetProposedScriptName(FText::FromString(Blueprint->GetName()));
	}
	
	FEditorDelegates::OnMapLoad.AddSP(this, &FBangoScriptContainerCustomization::OnMapLoad);
	
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
	if (PropertyHandle->GetNumOuterObjects() != 1)
	{
		return;
	}
	
	UObject* Outer = GetOuter();
	
	// TODO handle selection of multiple things
	if (Outer && Outer->IsA(UBangoScriptComponent::StaticClass()))
	{
		//return;
	}
	
	ChildBuilder.AddCustomRow(INVTEXT("ASDF"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("BangoScriptNameOverride_Label", "Name"))
		.TextStyle(FAppStyle::Get(), "SmallText")
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("BangoScriptNameOverride_HintText", "Optional"))
			.SelectAllTextWhenFocused(true)
			.ForegroundColor(this, &FBangoScriptContainerCustomization::ForegroundColor_ScriptNameEditableText)
			.FocusedForegroundColor(this, &FBangoScriptContainerCustomization::FocusedForegroundColor_ScriptNameEditableText)
			.MinDesiredWidth(160)
			.Text_Lambda( [this] () { return ScriptNameText; })
			.OnTextChanged(this, &FBangoScriptContainerCustomization::OnTextChanged_ScriptNameEditableText)
			//.ColorAndOpacity(this, &FBangoScriptContainerCustomization::ColorAndOpacity_ScriptNameText)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 0, 0, 0)
		[
			SNew(SButton)
			.IsEnabled(this, &FBangoScriptContainerCustomization::IsEnabled_RenameScriptButton)
			.VAlign(VAlign_Center)
			.Text(LOCTEXT("BangoScriptNameOverride_RenameButtonLabel", "Rename"))
			.TextStyle(FAppStyle::Get(), "SmallText")
			.OnClicked(this, &FBangoScriptContainerCustomization::OnClicked_RenameScript)
		]
	];
	
	ChildBuilder.AddCustomRow(INVTEXT("ASDF"))
	.NameContent()
	[
		GuidProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		GuidProperty->CreatePropertyValueWidgetWithCustomization(nullptr)
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
	
	void* ScriptContainerPtr = nullptr;
	ScriptContainerProperty->GetValueData(ScriptContainerPtr);
	
	FBangoScriptContainer* ScriptContainer = reinterpret_cast<FBangoScriptContainer*>(ScriptContainerPtr);
	
	//FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(GetOuter(), ScriptContainer);
	
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
		UBlueprint* ScriptBlueprint = UBlueprint::GetBlueprintFromClass(GetScriptClass());
		
		Subsystem->OpenEditorForAsset(ScriptBlueprint);
	}
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptContainerCustomization::OnClicked_EnlargeGraphView() const
{
//#if 0
	TSharedRef<FBangoBlueprintEditor> NewBlueprintEditor(new FBangoBlueprintEditor());

	NewBlueprintEditor->SetWarningText(INVTEXT("Test!"));
	
	const bool bShouldOpenInDefaultsMode = false;
	TArray<UBlueprint*> Blueprints;
	Blueprints.Add(GetBlueprint());

	NewBlueprintEditor->InitBlueprintEditor(EToolkitMode::Standalone, nullptr, Blueprints, bShouldOpenInDefaultsMode);
	//NewBlueprintEditor->InitWidgetBlueprintEditor(EToolkitMode::Standalone, nullptr, Blueprints, bShouldOpenInDefaultsMode);
	
	return FReply::Handled();
//#endif 

#if 0
	//return OnClicked_EditScript();
	
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
			return BangoColor::Gray;
		}
		case EBangoScriptRenameStatus::MatchesOther:
		{
			return BangoColor::LightRed;
		}
		case EBangoScriptRenameStatus::ValidNewName:
		{
			return BangoColor::LightGreen;
		}
		default:
		{
			return BangoColor::Error;
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

TSharedRef<SWidget> FBangoScriptContainerCustomization::GetPopoutGraphEditor() const
{
	SGraphEditor::FGraphEditorEvents Events;
	
	TSharedRef<FBangoBlueprintEditor> BlueprintEditor = MakeShared<FBangoBlueprintEditor>();
	//BlueprintEditor->InitBlueprintEditor(EToolkitMode::Standalone, nullptr, {GetBlueprint()}, true);
	BlueprintEditor->SetupGraphEditorEvents_Impl(GetBlueprint(), GetPrimaryEventGraph(), Events);
	
	//FBlueprintEditor_Private::Call_SetupGraphEditorEvents(GetPrimaryEventGraph(), lpr, Events);
	
	
	//UE_DEFINE_PRIVATE_MEMBER_PTR(void (UEdGraph*, SGraphEditor::FGraphEditorEvents&), Test, FBlueprintEditor, SetupGraphEditorEvents);
	//Test();
	
	const bool bShouldOpenInDefaultsMode = false;
	TArray<UBlueprint*> Blueprints;
	Blueprints.Add(GetBlueprint());

	//NewBlueprintEditor->InitWidgetBlueprintEditor(EToolkitMode::Standalone, nullptr, Blueprints, bShouldOpenInDefaultsMode);
	
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
					SNew(SBangoGraphEditor)
					.BlueprintEditor(BlueprintEditor)
					.GraphToEdit(GetPrimaryEventGraph())
					.IsEditable(true)
					.GraphEvents(Events)
					.ShowGraphStateOverlay(true)
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
	
	UBangoScript::SelectedScript = GetScriptClass()->GetPathName();
	
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
						FText::FromString( GetBlueprint()->GetPackage()->ContainsMap() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension())
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

void FBangoScriptContainerCustomization::OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap)
{
	// If we don't get rid of the blueprint graph, we get an editor crash when switching maps.
	Box->ClearChildren();
	CurrentGraph = nullptr;
}

// ----------------------------------------------

// ----------------------------------------------

#undef LOCTEXT_NAMESPACE