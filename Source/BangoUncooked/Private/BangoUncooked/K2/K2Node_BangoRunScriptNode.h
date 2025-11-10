#pragma once
#include "K2Node_BaseAsyncTask.h"
#include "Engine/CancellableAsyncAction.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "K2Node_BangoRunScriptNode.generated.h"

class UBangoScriptObject;

#define LOCTEXT_NAMESPACE "BangoEditor"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelayOutputPin);

UCLASS(MinimalAPI, DisplayName = "Run Script")
class UK2Node_BangoRunScript : public UK2Node//_BaseAsyncTask
{
    GENERATED_BODY()

protected:
	struct FBaseAsyncTaskHelper
    {
    	struct FOutputPinAndLocalVariable
    	{
    		UEdGraphPin* OutputPin;
    		UK2Node_TemporaryVariable* TempVar;

    		FOutputPinAndLocalVariable(UEdGraphPin* Pin, UK2Node_TemporaryVariable* Var) : OutputPin(Pin), TempVar(Var) {}
    	};

    	static bool ValidDataPin(const UEdGraphPin* Pin, EEdGraphPinDirection Direction);
    	static bool CreateDelegateForNewFunction(UEdGraphPin* DelegateInputPin, FName FunctionName, UK2Node* CurrentNode, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext);
    	static bool CopyEventSignature(UK2Node_CustomEvent* CENode, UFunction* Function, const UEdGraphSchema_K2* Schema);
    	static bool HandleDelegateImplementation(
			FMulticastDelegateProperty* CurrentProperty, const TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable>& VariableOutputs,
			UEdGraphPin* ProxyObjectPin, UEdGraphPin*& InOutLastThenPin, UEdGraphPin*& OutLastActivatedThenPin,
			UK2Node* CurrentNode, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext);

    	static const FName GetAsyncTaskProxyName();
    };
	
protected:
	UPROPERTY(EditAnywhere)
	bool bTest = false;
	
public:
	UK2Node_BangoRunScript();
	
    void PinDefaultValueChanged(UEdGraphPin* Pin) override;

    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	void PostLoad() override;

	void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	
	void PinConnectionListChanged(UEdGraphPin* Pin) override;
	
    UEdGraphPin* GetScriptPin(const TArray<UEdGraphPin*>* PinsToSearch = nullptr) const;
    
    UEdGraphPin* CachedScriptPin;

	bool IsLatentForMacros() const override
	{
		return true;
	}

	FName GetCornerIcon() const override
	{
		return TEXT("Graph.Latent.LatentIcon");
	}
	
	FText GetToolTipHeading() const override
	{
		return LOCTEXT("LatentFunc", "Latent");
	}
	
public:
	
	/** Executed when the node is either succeeded OR advanced. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Finished")
	FDelayOutputPin Finished;

    FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	FText GetTooltipText() const override;
	
    void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

    void AllocateDefaultPins() override;

	FText GetMenuCategory() const override;

	void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	
    UEdGraphPin* FindPropertyPin(const FName InPinName) const;
    
    UPROPERTY()
    TArray<FName> PinNames;

	void UpdateScriptPins(UObject* InClassObject);
};

#undef LOCTEXT_NAMESPACE