#include "BangoBlueprintEditor.h"

#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_Literal.h"
#include "SBlueprintEditorToolbar.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/DebuggerCommands.h"
#include "WorldPartition/ActorDescContainerInstance.h"
#include "WorldPartition/WorldPartition.h"

#define LOCTEXT_NAMESPACE "Bango"

void FBangoBlueprintEditor::SetupGraphEditorEvents(UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents)
{
	FBlueprintEditor::SetupGraphEditorEvents(InGraph, InEvents);
	InEvents.OnDropActors.Unbind();
	InEvents.OnDropActors = SGraphEditor::FOnDropActors::CreateSP(this, &FBangoBlueprintEditor::OnDropActors);
}

void FBangoBlueprintEditor::SetupGraphEditorEvents_Impl(UBlueprint* Blueprint, UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents)
{
	SetupGraphEditorEvents(InGraph, InEvents);
		
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP( this, &FBangoBlueprintEditor::OnSelectedNodesChanged );
	InEvents.OnDropActors = SGraphEditor::FOnDropActors::CreateSP( this, &FBangoBlueprintEditor::OnGraphEditorDropActor );
	InEvents.OnDropStreamingLevels = SGraphEditor::FOnDropStreamingLevels::CreateSP( this, &FBangoBlueprintEditor::OnGraphEditorDropStreamingLevel );
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FBangoBlueprintEditor::OnNodeDoubleClicked);
	InEvents.OnVerifyTextCommit = FOnNodeVerifyTextCommit::CreateSP(this, &FBangoBlueprintEditor::OnNodeVerifyTitleCommit);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FBangoBlueprintEditor::OnNodeTitleCommitted);
	InEvents.OnSpawnNodeByShortcutAtLocation = SGraphEditor::FOnSpawnNodeByShortcutAtLocation::CreateSP(this, &FBangoBlueprintEditor::OnSpawnGraphNodeByShortcut, InGraph);
	InEvents.OnNodeSpawnedByKeymap = SGraphEditor::FOnNodeSpawnedByKeymap::CreateSP(this, &FBangoBlueprintEditor::OnNodeSpawnedByKeymap );
	//InEvents.OnDisallowedPinConnection = SGraphEditor::FOnDisallowedPinConnection::CreateSP(this, &FBangoBlueprintEditor::OnDisallowedPinConnection);
	InEvents.OnDoubleClicked = SGraphEditor::FOnDoubleClicked::CreateSP(this, &FBangoBlueprintEditor::NavigateToParentGraphByDoubleClick);
		
	// Custom menu for K2 schemas
	if(InGraph->Schema != nullptr && InGraph->Schema->IsChildOf(UEdGraphSchema_K2::StaticClass()))
	{
		InEvents.OnCreateActionMenuAtLocation = SGraphEditor::FOnCreateActionMenuAtLocation::CreateSP(this, &FBangoBlueprintEditor::OnCreateGraphActionMenu);
	}
}

void FBangoBlueprintEditor::Tick(float DeltaTime)
{
	if (bRequestedSavingOpenDocumentState)
	{
		bRequestedSavingOpenDocumentState = false;

		SaveEditedObjectState();
	}

	OpenTime += DeltaTime;

	if (bPendingDeferredClose)
	{
		IAssetEditorInstance* EditorInst = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(GetBlueprintObj(), /*bFocusIfOpen =*/false);
		check(EditorInst != nullptr);
		EditorInst->CloseWindow(EAssetEditorCloseReason::AssetUnloadingOrInvalid);
	}
	else
	{
		// Auto-import any namespaces we've collected as a result of compound events that may have occurred within this frame.
		if (!DeferredNamespaceImports.IsEmpty())
		{
			FImportNamespaceExParameters Params;
			Params.NamespacesToImport = MoveTemp(DeferredNamespaceImports);
			ImportNamespaceEx(Params);

			// Assert that this was reset by the move above.
			check(DeferredNamespaceImports.IsEmpty());
		}
	}
}

void FBangoBlueprintEditor::OnDropActors(const TArray<TWeakObjectPtr<AActor>>& Actors, UEdGraph* Graph, const UE::Math::TVector2<float>& DropLocation) const
{
	UE_LOG(LogBango, Display, TEXT("Test 2"));
	
	FDeprecateSlateVector2D NodeLocation = DropLocation;
	
	for (int32 i = 0; i < Actors.Num(); i++)
	{
		AActor* DroppedActor = Actors[i].Get();
		if (DroppedActor && /*(DroppedActor->GetLevel() == BlueprintLevel) &&*/ !DroppedActor->IsChildActor())
		{
			UK2Node_BangoFindActor* ActorFindNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_BangoFindActor>(
				Graph, 
				NodeLocation, 
				EK2NewNodeFlags::SelectNewNode, 
				[DroppedActor] (UK2Node_BangoFindActor* NewNode)
				{
					NewNode->SetActor(DroppedActor);
				});
		}
	}
}

FGraphAppearanceInfo FBangoBlueprintEditor::GetGraphAppearance(class UEdGraph* InGraph) const
{
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;

	FText OwnerNameText = GetOwnerNameAsText();
	FText LevelNameText = GetLevelNameAsText();
	
	FText InstructionText = OwnerNameText.IsEmpty() ? OwnerNameText : FText::Format(INVTEXT("{0} \U00002014 {1}"), { OwnerNameText, LevelNameText.IsEmpty() ? LOCTEXT("ScriptBlueprintEditor_NoLevel", "No Level") : LevelNameText }) ;
	FText CornerText = LevelNameText.IsEmpty() ? LOCTEXT("ScriptBlueprintEditor_ContentScript", "Content Script") : LOCTEXT("ScriptBlueprintEditor_LevelScript", "Level Script");
	
	AppearanceInfo.CornerText = CornerText;
	AppearanceInfo.InstructionText = InstructionText;
	AppearanceInfo.PIENotifyText = GetPIEStatus();
	AppearanceInfo.WarningText = WarningText;

	const float StartTimeNoFade = 2.0f;
	const float FadeDuration = 4.0f;
	const float MinFade = 0.15f;
	
	float Alpha = FMath::LerpStable(1.0f, 0.0f, (OpenTime - StartTimeNoFade) / FadeDuration);
	Alpha = FMath::Clamp(Alpha, MinFade, 1.0f);
	
	AppearanceInfo.InstructionFade = Alpha; 
	
	// Doesn't do anything. Unimplemented UE feature.
	//AppearanceInfo.CornerImage = ???

	return AppearanceInfo;
}

FText FBangoBlueprintEditor::GetLevelNameAsText() const
{
	UBangoScriptBlueprint* Blueprint = Cast<UBangoScriptBlueprint>(GetBlueprintObj());
	
	if (!Blueprint)
	{
		return FText::GetEmpty();
	}
	
	TSoftObjectPtr<AActor> Actor = Blueprint->GetActor();
	
	if (Actor.IsNull())
	{
		return FText::GetEmpty();
	}
	else if (Actor.IsValid())
	{
		UPackage* LevelPackage = Actor->GetLevel()->GetPackage();
		return FText::FromString(FPackageName::GetShortName(LevelPackage) + FPackageName::GetMapPackageExtension());
	}
	else
	{
		// Try to get it from world partition
		UWorld* World = GEditor->EditorWorld;
		
		if (World)
		{
			UWorldPartition* WorldPartition = World->GetWorldPartition();
	
			if (WorldPartition)
			{
				UActorDescContainerInstance* ActorDescContainer = WorldPartition->GetActorDescContainerInstance();
		
				if (ActorDescContainer)
				{
					const FWorldPartitionActorDescInstance* ActorDesc = ActorDescContainer->GetActorDescInstanceByPath(Actor.ToSoftObjectPath());
			
					if (ActorDesc)
					{
						UPackage* LevelPackage = World->PersistentLevel.GetPackage();
						return FText::FromString(FPackageName::GetShortName(LevelPackage) + FPackageName::GetMapPackageExtension());
					}
				}
			}
		}
		
		// Unknown/unloaded fallback
		return LOCTEXT("OwnerName_UnloadedActor", "Unloaded Level");
	}
}

FText FBangoBlueprintEditor::GetOwnerNameAsText() const
{
	UBangoScriptBlueprint* Blueprint = Cast<UBangoScriptBlueprint>(GetBlueprintObj());
	
	if (!Blueprint)
	{
		return FText::GetEmpty();
	}
	
	TSoftObjectPtr<AActor> Actor = Blueprint->GetActor();
	
	if (Actor.IsNull())
	{
		return FText::GetEmpty();
	}
	else if (Actor.IsValid())
	{
		return FText::FromString(Actor->GetActorLabel());
	}
	else
	{
		// Try to get it from world partition
		UWorld* World = GEditor->EditorWorld;
		
		if (World)
		{
			UWorldPartition* WorldPartition = World->GetWorldPartition();
	
			if (WorldPartition)
			{
				UActorDescContainerInstance* ActorDescContainer = WorldPartition->GetActorDescContainerInstance();
		
				if (ActorDescContainer)
				{
					const FWorldPartitionActorDescInstance* ActorDesc = ActorDescContainer->GetActorDescInstanceByPath(Actor.ToSoftObjectPath());
			
					if (ActorDesc)
					{
						return FText::FromName(ActorDesc->GetActorLabel());
					}
				}
			}
		}
		
		// Unknown/unloaded fallback
		return LOCTEXT("OwnerName_UnloadedActor", "Unloaded Actor");
	}
}

void FBangoBlueprintEditor::SetWarningText(const FText& InText)
{
	WarningText = InText;
}

void FBangoBlueprintEditor::InitBangoBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	check(InBlueprints.Num() == 1 || bShouldOpenInDefaultsMode);

	// TRUE if a single Blueprint is being opened and is marked as newly created
	bool bNewlyCreated = InBlueprints.Num() == 1 && InBlueprints[0]->bIsNewlyCreated;

	// Load editor settings from disk.
	// LoadEditorSettings();

	TArray< UObject* > Objects;
	for (UBlueprint* Blueprint : InBlueprints)
	{
		// Flag the blueprint as having been opened
		Blueprint->bIsNewlyCreated = false;

		Objects.Add( Blueprint );
	}
	
	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}

	GetToolkitCommands()->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());

	CreateDefaultCommands();

	RegisterMenus();

	// Initialize the asset editor and spawn nothing (dummy layout)
	//const bool bCreateDefaultStandaloneMenu = true;
	//const bool bCreateDefaultToolbar = true;
	//const FName BlueprintEditorAppName = FName(TEXT("BlueprintEditorApp"));
	// InitAssetEditor(Mode, InitToolkitHost, BlueprintEditorAppName, FTabManager::FLayout::NullLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Objects);

	///
	AddEditingObject( InBlueprints[0] );
	///
	
	CommonInitialization(InBlueprints, bShouldOpenInDefaultsMode);

	InitalizeExtenders();

	RegenerateMenusAndToolbars();

	RegisterApplicationModes(InBlueprints, bShouldOpenInDefaultsMode, bNewlyCreated);

	// Post-layout initialization
	// PostLayoutBlueprintEditorInitialization();

	// Find and set any instances of this blueprint type if any exists and we are not already editing one
	FBlueprintEditorUtils::FindAndSetDebuggableBlueprintInstances();

	if ( bNewlyCreated )
	{
		if ( UBlueprint* Blueprint = GetBlueprintObj() )
		{
			if ( Blueprint->BlueprintType == BPTYPE_MacroLibrary )
			{
				NewDocument_OnClick(CGT_NewMacroGraph);
			}
			else if ( Blueprint->BlueprintType == BPTYPE_Interface )
			{
				NewDocument_OnClick(CGT_NewFunctionGraph);
			}
			else if ( Blueprint->BlueprintType == BPTYPE_FunctionLibrary )
			{
				NewDocument_OnClick(CGT_NewFunctionGraph);
			}
		}
	}

	if ( UBlueprint* Blueprint = GetBlueprintObj() )
	{
		if ( Blueprint->GetClass() == UBlueprint::StaticClass() && Blueprint->BlueprintType == BPTYPE_Normal )
		{
			if ( !bShouldOpenInDefaultsMode )
			{
				GetToolkitCommands()->ExecuteAction(FFullBlueprintEditorCommands::Get().EditClassDefaults.ToSharedRef());
			}
		}

		// There are upgrade notes, open the log and dump the messages to it
		if (Blueprint->UpgradeNotesLog.IsValid())
		{
			DumpMessagesToCompilerLog(Blueprint->UpgradeNotesLog->Messages, true);
		}
	}

	/*
	// Register for notifications when settings change
	BlueprintEditorSettingsChangedHandle = GetMutableDefault<UBlueprintEditorSettings>()->OnSettingChanged()
		.AddRaw(this, &FBlueprintEditor::OnBlueprintEditorPreferencesChanged);
	BlueprintProjectSettingsChangedHandle = GetMutableDefault<UBlueprintEditorProjectSettings>()->OnSettingChanged()
		.AddRaw(this, &FBlueprintEditor::OnBlueprintProjectSettingsChanged);
	*/
	
	/*
	if (const TSharedPtr<SSCSEditorViewport> Viewport = GetSubobjectViewport())
	{
		ViewportSelectabilityBridge = MakeUnique<FEditorViewportSelectabilityBridge>(Viewport->GetViewportClient());
	}
	*/
}

void FBangoBlueprintEditor::AddEditingObject(UObject* Object)
{
	FBlueprintEditor::AddEditingObject(Object);
}

void FBangoBlueprintEditor::SetCurrentMode(FName NewMode)
{
	FBlueprintEditor::SetCurrentMode(NewMode);
	//SetUISelectionState(NAME_None);
}

void FBangoBlueprintEditor::PostInitAssetEditor()
{
	FBlueprintEditor::PostInitAssetEditor();
	
	OpenTime = 0.0f;
}

void FBangoBlueprintEditor::PasteGeneric()
{
	FBlueprintEditor::PasteGeneric();
}

/*
FActionMenuContent FBangoBlueprintEditor::OnCreateGraphActionMenu_Impl(UEdGraph* InGraph, const FVector2f& InNodePosition, const TArray<UEdGraphPin*>& InDraggedPins, bool bAutoExpand, SGraphEditor::FActionMenuClosed InOnMenuClosed)
{
	HasOpenActionMenu = InGraph;
	if (!BlueprintEditorImpl::GraphHasUserPlacedNodes(InGraph))
	{
		InstructionsFadeCountdown = BlueprintEditorImpl::InstructionFadeDuration;
	}

	TSharedRef<SBlueprintActionMenu> ActionMenu = 
	SNew(SBlueprintActionMenu, SharedThis(this))
	.GraphObj(InGraph)
	.NewNodePosition(FDeprecateSlateVector2D(InNodePosition))
	.DraggedFromPins(InDraggedPins)
	.AutoExpandActionMenu(bAutoExpand)
	.OnClosedCallback(InOnMenuClosed)
	.OnCloseReason(this, &FBlueprintEditor::OnGraphActionMenuClosed);

	return FActionMenuContent( ActionMenu, ActionMenu->GetFilterTextBox() );
}
*/

#undef LOCTEXT_NAMESPACE