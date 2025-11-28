#include "BangoEditor/Customizations/Properties/BangoScriptHolderCustomization.h"

#include "BlueprintEditor.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "SEditorViewport.h"
#include "Bango/Core/BangoScriptObject.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

// ----------------------------------------------

FBangoScriptHolderCustomization::FBangoScriptHolderCustomization()
{
	PostScriptCreated.AddRaw(this, &FBangoScriptHolderCustomization::OnPostScriptCreated);
	PreScriptDeleted.AddRaw(this, &FBangoScriptHolderCustomization::OnPreScriptDeleted);
}

// ----------------------------------------------

TSharedRef<IPropertyTypeCustomization> FBangoScriptHolderCustomization::MakeInstance()
{
	return MakeShared<FBangoScriptHolderCustomization>();
}

// ----------------------------------------------

UEdGraph* FBangoScriptHolderCustomization::GetPrimaryEventGraph() const
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

void FBangoScriptHolderCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ScriptBlueprintProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptHolder, ScriptBlueprint));
	ScriptClassProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBangoScriptHolder, ScriptClass));
	
	CurrentGraph = GetPrimaryEventGraph();
	
	HeaderRow.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("BangoScriptHolder_ScriptPropertyLabel", "Bango Script"))
	];
	
	HeaderRow.ValueContent()
	[
		SNew(SWidgetSwitcher)
		.WidgetIndex(this, &FBangoScriptHolderCustomization::WidgetIndex_CreateDeleteScriptButtons)
		+ SWidgetSwitcher::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("BangoScriptHolder_CreateScriptButtonText", "None (Click to Create)"))
			.OnClicked(this, &FBangoScriptHolderCustomization::OnClicked_CreateScript)
		]
		+ SWidgetSwitcher::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.AutoWrapText(false)
				.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				.Text_Lambda( [this] ()
				{
					UClass* ScriptClass = GetScriptClass();
				
					if (ScriptClass)
					{
						return FText::Format(INVTEXT("Script: {0}"), FText::FromString(ScriptClass->GetName()));	
					}
				
					return INVTEXT("Invalid/Unset");
				})
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(LOCTEXT("BangoScriptHolder_DeleteScriptButtonText", "Delete"))
				.OnClicked(this, &FBangoScriptHolderCustomization::OnClicked_DeleteScript)
			]
		]
	];
	
}

int FBangoScriptHolderCustomization::WidgetIndex_CreateDeleteScriptButtons() const
{
	if (IsValid(GetScriptClass()))
	{
		return 1;
	}
	
	return 0;
}

// ----------------------------------------------

void FBangoScriptHolderCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UpdateBox();
	
	ChildBuilder.AddCustomRow(LOCTEXT("BangoScriptHolder_SearchTerm", "Bango"))
	[
		Box.ToSharedRef()
	];
}

int FBangoScriptHolderCustomization::WidgetIndex_GraphEditor() const
{
	return 0;
}

// ----------------------------------------------

FReply FBangoScriptHolderCustomization::OnClicked_CreateScript()
{
	TArray<UPackage*> Packages;
	
	ScriptClassProperty->GetOuterPackages(Packages);
	
	const UClass* OuterBaseClass = ScriptClassProperty->GetOuterBaseClass();

	if (Packages.Num() != 1)
	{
		return FReply::Handled();
	}

	FString BPName;
	UPackage* LocalScriptPackage = MakeLocalScriptPackage(Packages[0], BPName);
	
	UBlueprint* Script = FKismetEditorUtilities::CreateBlueprint(UBangoScriptObject::StaticClass(), LocalScriptPackage, FName(BPName), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	Script->SetFlags(RF_Public);
	Script->Modify();
	Script->MarkPackageDirty();
	LocalScriptPackage->SetDirtyFlag(true);
	
	FKismetEditorUtilities::CompileBlueprint(Script);
	
	auto x = ScriptBlueprintProperty->SetValue(Script);
	auto y = ScriptClassProperty->SetValue(Script->GeneratedClass);
	
	AActor* asdf = GetOwner();
	
	for (TFieldIterator<FProperty> It(asdf->GetClass()); It; ++It)
	{
		UE_LOG(LogTemp, Display, TEXT("Prop: %s"), *FString(It->GetName()));
	}
	
	const FString PackageName = LocalScriptPackage->GetName();
	FString ExistingFilename;
	const bool bPackageAlreadyExists = FPackageName::DoesPackageExist(PackageName, &ExistingFilename);
		
	bool bAttemptSave = true;
        			
	if (!bPackageAlreadyExists)
	{
		const FString& FileExtension = FPackageName::GetAssetPackageExtension();
		ExistingFilename = FPackageName::LongPackageNameToFilename(PackageName, FileExtension);
			
		// Check if we can use this filename.
		FText ErrorText;
		if (!FFileHelper::IsFilenameValidForSaving(ExistingFilename, ErrorText))
		{
			// Display the error (already localized) and exit gracefully.
			FMessageDialog::Open(EAppMsgType::Ok, ErrorText);
			bAttemptSave = false;
		}
	}
		
	if (bAttemptSave)
	{
		FString BaseFilename, Extension, Directory;
		FPaths::NormalizeFilename(ExistingFilename);
		FPaths::Split(ExistingFilename, Directory, BaseFilename, Extension);
			
		FString FinalPackageSavePath = ExistingFilename;
			
		FString FinalPackageFilename = FString::Printf(TEXT("%s.%s"), *BaseFilename, *Extension);
			
		FSavePackageArgs SaveArgs;
		bool bResult = UPackage::SavePackage(LocalScriptPackage, Script, /* *PackagePath2 */ *FinalPackageSavePath, SaveArgs);
	}
	
	PostScriptCreated.Broadcast();
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptHolderCustomization::OnClicked_DeleteScript()
{
	PreScriptDeleted.Broadcast();
	
	return FReply::Handled();
}

// ----------------------------------------------

FReply FBangoScriptHolderCustomization::OnClicked_EditScript()
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

FReply FBangoScriptHolderCustomization::OnClicked_EnlargeGraphView() const
{
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
}

// ----------------------------------------------

TSharedRef<SWidget> FBangoScriptHolderCustomization::GetPopoutGraphEditor() const
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

void FBangoScriptHolderCustomization::OnPostScriptCreated()
{
	CurrentGraph = GetPrimaryEventGraph();
	
	UpdateBox();	
}

void FBangoScriptHolderCustomization::OnPreScriptDeleted()
{
	CurrentGraph = nullptr;
	
	UpdateBox();
}

void FBangoScriptHolderCustomization::UpdateBox()
{
	SGraphEditor::FGraphEditorEvents Events;
	
	if (CurrentGraph.IsValid())
	{
		Box = SNew(SBox)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.HeightOverride(200)
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
				.OnClicked(this, &FBangoScriptHolderCustomization::OnClicked_EnlargeGraphView)
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.Fullscreen"))
				]
			]
		];
	}
	else
	{
		Box = SNew(STextBlock)
			.Text(LOCTEXT("BangoScriptHolder_NoScriptGraphLabel", "No script"));
	}
}

// ----------------------------------------------

AActor* FBangoScriptHolderCustomization::GetOwner() const
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

// ----------------------------------------------

UBlueprint* FBangoScriptHolderCustomization::GetBlueprint() const
{
	UObject* BlueprintObject;
	
	if (ScriptBlueprintProperty->GetValue(BlueprintObject) == FPropertyAccess::Success)
	{
		return Cast<UBlueprint>(BlueprintObject);
	}
	
	return nullptr;
}

// ----------------------------------------------

TSubclassOf<UBangoScriptObject> FBangoScriptHolderCustomization::GetScriptClass() const
{
	UObject* ClassObject;
	
	if (ScriptClassProperty->GetValue(ClassObject) == FPropertyAccess::Success)
	{
		return Cast<UClass>(ClassObject);
	}

	return nullptr;
}

// ----------------------------------------------

UPackage* FBangoScriptHolderCustomization::MakeLocalScriptPackage(UObject* Outer, FString& NewBPName)
{
	FGuid UniqueID = FGuid::NewGuid();
	
	// Usually an OFP package in __ExternalActors__

	FString FolderShortName = FString("LocalBangoScript") + TEXT("_UID_") + UniqueID.ToString(EGuidFormats::UniqueObjectGuid);
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> GloballyUniqueObjectPath;
	GloballyUniqueObjectPath += GetOwner() ? GetOwner()->GetLevel()->GetPathName() : "";
	GloballyUniqueObjectPath += TEXT(".");
	GloballyUniqueObjectPath += FolderShortName;
	
	const UPackage* OutermostPackage = Outer->IsA<UPackage>() ? CastChecked<UPackage>(Outer) : Outer->GetOutermostObject()->GetPackage();
	const FString RootPath = OutermostPackage->GetName();
	const FString ExternalObjectPackageName = FBangoPackageHelper::GetLocalScriptPackageName(RootPath, FolderShortName);

	FString FinalPath = ExternalObjectPackageName;// + FPackageName::GetAssetPackageExtension();
	
	NewBPName = FolderShortName;
	return CreatePackage(*FinalPath);
}

// ----------------------------------------------

#undef LOCTEXT_NAMESPACE