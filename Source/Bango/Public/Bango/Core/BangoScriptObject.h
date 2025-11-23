// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once
#include "BangoScriptHandle.h"

#include "BangoScriptObject.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FWaitUntilDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLatentActionDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnLatentActionTick);
DECLARE_DYNAMIC_DELEGATE(FOnLatentActionCompleted);

#define LOCTEXT_NAMESPACE "Bango"

using DataValidationDelegate = TDelegate<EDataValidationResult(class FDataValidationContext& Context, const UBangoScriptObject* ScriptObject)>;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BANGO_API UBangoScriptObject : public UObject
{
    GENERATED_BODY()

    friend class UBangoScriptValidator;
    
protected:
    // TODO: is this a bad decision? How else can I do this? Can I register things to keep scripts alive? Can I discover delegate subs in blueprints?
    /** By default, Bango will destroy script objects once they stop running any latent actions. You need to turn this on if a script needs to wait for external events, such as subscribing to a delegate of another Actor. */
    UPROPERTY(EditAnywhere)
    bool bPreventAutoDestroy = false;
    
    bool GetKeepAliveWhenIdle() const { return bPreventAutoDestroy; }
    
public:
    /** This is called by Bango. */ 
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    FBangoScriptHandle Execute_Internal();
    
    /** This is implemented by designers. */
    UFUNCTION(BlueprintImplementableEvent)
    void Start();

    /** This is supposed to be called at the end of the Execute function */
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "Script", BlueprintProtected))
    static void Finish(UBangoScriptObject* Script);

#if WITH_EDITOR
    bool ImplementsGetWorld() const override { return true; }
#endif
    
    UPROPERTY(BlueprintAssignable)
    FOnFinishDelegate OnFinishDelegate;

    UPROPERTY(Transient)
    FBangoScriptHandle Handle;

    UPROPERTY(Transient)
    TMap<int32, FOnLatentActionTick> SleepTickDelegates;
    TMap<int32, FOnLatentActionCompleted> SleepCancelDelegates;

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="1.23", Keywords="sleep"))
    static UPARAM(DisplayName = "UUID") int32 LaunchSleep_Internal(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo, FOnLatentActionTick TickDelegate, FOnLatentActionCompleted CompleteDelegate);

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", Keywords="sleep"))
    static void CancelSleep_Internal(UObject* WorldContextObject, int32 ActionUUID);

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", Keywords="sleep"))
    static void SkipSleep_Internal(UObject* WorldContextObject, int32 ActionUUID);

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", Keywords="sleep"))
    static void SetSleepPause_Internal(UObject* WorldContextObject, bool bPaused, int32 ActionUUID);
    
    UFUNCTION(BlueprintCallable, Category="Bango|Utilities", BlueprintPure)
    static UPARAM(DisplayName="Val") float Rand(float Hi, float Lo);
    
#if WITH_EDITOR
    
    friend class UBangoScriptValidator;

    EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
    
protected:
    static DataValidationDelegate OnScriptRequestValidation;
    
#endif
};

#undef LOCTEXT_NAMESPACE