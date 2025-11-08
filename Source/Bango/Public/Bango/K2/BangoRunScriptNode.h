#pragma once
#include "K2Node_BaseAsyncTask.h"
#include "Engine/CancellableAsyncAction.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "BangoRunScriptNode.generated.h"

UCLASS(MinimalAPI, meta = (ExposedAsyncProxy = "AsyncAction", HasDedicatedAsyncNode)) 
class UBangoRunScript : public UCancellableAsyncAction
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBangoRunScriptOutputPin);

    UPROPERTY(BlueprintAssignable, Category="Bango|Test")
    FBangoRunScriptOutputPin OnSomeEvent;
    
    virtual void Activate() override;
    virtual void Cancel() override;
    virtual void SetReadyToDestroy() override;

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true", Category = "Bango|Test"))
    static UBangoRunScript* RunScript(UObject* WorldContext) { return NewObject<UBangoRunScript>(); };

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true", Category = "Bango|Test"))
    static UBangoRunScript* WaitForScript(UObject* WorldContext) { return NewObject<UBangoRunScript>(); };

};
