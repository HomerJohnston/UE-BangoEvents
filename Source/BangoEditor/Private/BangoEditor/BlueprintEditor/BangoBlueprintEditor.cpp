#include "BangoBlueprintEditor.h"

#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_Literal.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Utility/BangoLog.h"
#include "BangoEditor/BangoEditorStyle.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"
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
	AddEditingObject(Blueprint);
	
	//SetupGraphEditorEvents(InGraph, InEvents);
		
	//InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP( this, &FBlueprintEditor::OnSelectedNodesChanged );
	//InEvents.OnDropActors = SGraphEditor::FOnDropActors::CreateSP( this, &FBlueprintEditor::OnGraphEditorDropActor );
	//InEvents.OnDropStreamingLevels = SGraphEditor::FOnDropStreamingLevels::CreateSP( this, &FBlueprintEditor::OnGraphEditorDropStreamingLevel );
	//InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FBlueprintEditor::OnNodeDoubleClicked);
	//InEvents.OnVerifyTextCommit = FOnNodeVerifyTextCommit::CreateSP(this, &FBlueprintEditor::OnNodeVerifyTitleCommit);
	//InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FBlueprintEditor::OnNodeTitleCommitted);
	//InEvents.OnSpawnNodeByShortcutAtLocation = SGraphEditor::FOnSpawnNodeByShortcutAtLocation::CreateSP(this, &FBlueprintEditor::OnSpawnGraphNodeByShortcut, InGraph);
	//InEvents.OnNodeSpawnedByKeymap = SGraphEditor::FOnNodeSpawnedByKeymap::CreateSP(this, &FBlueprintEditor::OnNodeSpawnedByKeymap );
	//InEvents.OnDisallowedPinConnection = SGraphEditor::FOnDisallowedPinConnection::CreateSP(this, &FBlueprintEditor::OnDisallowedPinConnection);
	//InEvents.OnDoubleClicked = SGraphEditor::FOnDoubleClicked::CreateSP(this, &FBlueprintEditor::NavigateToParentGraphByDoubleClick);
		
	// Custom menu for K2 schemas
	if(InGraph->Schema != nullptr && InGraph->Schema->IsChildOf(UEdGraphSchema_K2::StaticClass()))
	{
		InEvents.OnCreateActionMenuAtLocation = SGraphEditor::FOnCreateActionMenuAtLocation::CreateSP(this, &FBangoBlueprintEditor::OnCreateGraphActionMenu);
	}
	
	InEvents.OnDropActor = SGraphEditor::FOnDropActor::CreateSP(this, &FBangoBlueprintEditor::OnDropActor);
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

void FBangoBlueprintEditor::OnDropActor(const TArray<TWeakObjectPtr<AActor>>& Actors, UEdGraph* EdGraph, const UE::Math::TVector2<double>& Vector2) const
{
	UE_LOG(LogBango, Display, TEXT("Test 1"));
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

	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_Blueprint", "SCRIPT");
	AppearanceInfo.InstructionText = GetOwnerNameAsText();
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

FText FBangoBlueprintEditor::GetOwnerNameAsText() const
{
	UBangoScriptBlueprint* Blueprint = Cast<UBangoScriptBlueprint>(GetBlueprintObj());
	
	if (!Blueprint)
	{
		return FText::GetEmpty();
	}
	
	TSoftObjectPtr<AActor> Actor = Blueprint->GetActor();
	FText LevelActorTextFormat = LOCTEXT("OwnerNameText_ScriptGraph", "{0}: {1}");
	
	if (Actor.IsNull())
	{
		return LOCTEXT("OwnerName_NoActor", "Standalone Script");
	}
	else if (Actor.IsValid())
	{
		UPackage* LevelPackage = Actor->GetLevel()->GetPackage();
		return FText::Format(LevelActorTextFormat, { FText::FromString(FPackageName::GetShortName(LevelPackage)), FText::FromString(Actor->GetActorLabel()) } );
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
						return FText::Format(LevelActorTextFormat, { FText::FromString(FPackageName::GetShortName(LevelPackage)), FText::FromName(ActorDesc->GetActorLabel()) } );
					}
				}
			}
		}
		
		// Unknown/unloaded fallback
		return LOCTEXT("OwnerName_UnloadedActor", "Unknown/Unloaded Actor");
	}
}

void FBangoBlueprintEditor::SetWarningText(const FText& InText)
{
	WarningText = InText;
}

void FBangoBlueprintEditor::PostInitAssetEditor()
{
	FBlueprintEditor::PostInitAssetEditor();
	
	OpenTime = 0.0f;
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