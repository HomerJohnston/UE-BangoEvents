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
	/** Change to the desired class to cast the output automatically. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CastTo;
	
public:
	TSubclassOf<AActor> GetCastTo() const { return CastTo; }
	
public:
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void AllocateDefaultPins() override;
	
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;
	
	bool IsNodePure() const override { return true; }
};

#undef LOCTEXT_NAMESPACE