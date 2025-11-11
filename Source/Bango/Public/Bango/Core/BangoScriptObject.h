// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once
#include "BangoScriptHandle.h"

#include "BangoScriptObject.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FWaitUntilDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishDelegate);

#define LOCTEXT_NAMESPACE "Bango"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class UBangoScriptObject : public UObject
{
    GENERATED_BODY()

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

    bool ImplementsGetWorld() const override { return true; }

    UPROPERTY(BlueprintAssignable)
    FOnFinishDelegate OnFinishDelegate;

    UPROPERTY()
    FBangoScriptHandle Handle;

    /*
    UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="0.2", Keywords="sleep"))
    static void Sleep(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo );
    */

    /*
    UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="0.2", Keywords="sleep"))
    static void WaitUntil(bool bCondition) {};
    */
    
    UFUNCTION(BlueprintCallable, Category="Bango|Utilities", BlueprintPure)
    static UPARAM(DisplayName="Val") float Rand(float Hi, float Lo);
};

class FBangoSleepAction : public FPendingLatentAction
{
public:
    float Duration;
    float TimeRemaining;
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    TMulticastDelegate<void()> OnAborted;

    FBangoSleepAction(float InDuration, const FLatentActionInfo& LatentInfo)
        : Duration(InDuration)
        , TimeRemaining(InDuration)
        , ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
    {
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        TimeRemaining -= Response.ElapsedTime();
        Response.FinishAndTriggerIf(TimeRemaining <= 0.0f, ExecutionFunction, OutputLink, CallbackTarget);
    }

    virtual void NotifyActionAborted()
    {
        OnAborted.Broadcast();
    }
    
#if WITH_EDITOR
    // Returns a human readable description of the latent operation's current state
    virtual FString GetDescription() const override
    {
        static const FNumberFormattingOptions SleepTimeFormatOptions = FNumberFormattingOptions()
            .SetMinimumFractionalDigits(2)
            .SetMaximumFractionalDigits(2);
        return FText::Format(LOCTEXT("SleepActionTimeFmt", "{0} / {1}"),
            FText::AsNumber(Duration - TimeRemaining, &SleepTimeFormatOptions),
            FText::AsNumber(Duration, &SleepTimeFormatOptions)).ToString();
    }
#endif
};

#undef LOCTEXT_NAMESPACE