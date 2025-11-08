#pragma once
#include "K2Node_BaseAsyncTask.h"
#include "Engine/CancellableAsyncAction.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "K2Node_BangoRunScriptNode.generated.h"

UCLASS(MinimalAPI, DisplayName = "Run Script")
class UK2Node_BangoRunScript : public UK2Node_BaseAsyncTask
{
    GENERATED_BODY()

    void PinDefaultValueChanged(UEdGraphPin* Pin) override;

    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
    
    UEdGraphPin* GetScriptPin() const;
    
    UEdGraphPin* CachedScriptPin;

    FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    
    void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

    void AllocateDefaultPins() override;

    UEdGraphPin* FindPropertyPin(const FName InPinName) const;
    
    UPROPERTY()
    TArray<FName> PinNames;
};
