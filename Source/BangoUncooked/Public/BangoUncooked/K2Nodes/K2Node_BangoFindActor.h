#pragma once

#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "K2Node_BangoFindActor.generated.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UCLASS(MinimalAPI, DisplayName = "FindActor")
class UK2Node_BangoFindActor : public UK2Node_BangoBase
{
	GENERATED_BODY()

public:
	UK2Node_BangoFindActor();

protected:	
	/** This node will *either* target a manual name with CastTo, OR it will use TargetActor below. */
	UPROPERTY(VisibleAnywhere)
	FName TargetName;
	
	/** Change to the desired class to cast the output automatically. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "TargetName != NAME_None", EditConditionHides))
	TSubclassOf<AActor> CastTo;
	
	/** Level-blueprint-like actor reference, used by dragging an actor onto the blueprint from the world outliner. */
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<AActor> TargetActor;
	
	/** This is set automatically when TargetActor is set. If this is set, TargetName should NOT be set, and vice versa. */
	UPROPERTY(VisibleAnywhere)
	FGuid TargetBangoGuid;
	
	/** If the actor is unloaded, this can display the last known name. */
	UPROPERTY()
	FString CachedActorLabel;
	
	/** If the actor is unloaded, this can display the last known name. */
	UPROPERTY()
	FString CachedBangoName;
	
public:
	TSubclassOf<AActor> GetCastTo() const { return CastTo; }
	
	TSoftObjectPtr<AActor> GetTargetActor() const { return TargetActor; } 
	
	//FName GetTargetActorID() const { return TargetActorID; }
	
	bool ShouldDrawCompact() const override;
	
	FLinearColor GetNodeBodyTintColor() const override;
	
	FLinearColor GetNodeTitleColor() const override;
	
	FLinearColor GetNodeTitleTextColor() const override;
	
public:
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void AllocateDefaultPins() override;
	
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;
	
	bool IsNodePure() const override { return true; }
	
	BANGOUNCOOKED_API void SetActor(AActor* Actor);
	
	void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
	
	AActor* GetReferencedLevelActor() const override;
	
	void ReconstructNode() override;
	
	void PreloadRequiredAssets() override;
	
	void PostReconstructNode() override;
	
	//void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	
	void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
};

#undef LOCTEXT_NAMESPACE