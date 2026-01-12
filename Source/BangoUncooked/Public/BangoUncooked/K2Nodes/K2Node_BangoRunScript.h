#pragma once
#include "K2Node_BaseAsyncTask.h"
#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "K2Node_BangoRunScript.generated.h"

class UBangoScript;

#define LOCTEXT_NAMESPACE "BangoEditor"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelayOutputPin);

UCLASS(MinimalAPI, DisplayName = "Run Script")
class UK2Node_BangoRunScript : public UK2Node_BangoBase
{
    GENERATED_BODY()
	
public:
	UK2Node_BangoRunScript();
	
    void PinDefaultValueChanged(UEdGraphPin* Pin) override;

    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	void PostLoad() override;

	void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	
	void PinConnectionListChanged(UEdGraphPin* Pin) override;
	
    UEdGraphPin* GetScriptPin(const TArray<UEdGraphPin*>* PinsToSearch = nullptr) const;
    
    UEdGraphPin* CachedScriptPin;
	
public:
	/** Executed when the node is either succeeded OR advanced. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Finished")
	FDelayOutputPin Finished;

    FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	FText GetTooltipText() const override;
	
    void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

    void AllocateDefaultPins() override;

	void ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph) override;
	
    UEdGraphPin* FindPropertyPin(const FName InPinName) const;
    
    UPROPERTY()
    TArray<FName> PinNames;

	void UpdateScriptPins(UObject* InClassObject);
};

#undef LOCTEXT_NAMESPACE