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

    UPROPERTY(Transient)
    FBangoScriptHandle Handle;

    UPROPERTY(Transient)
    TMap<int32, FOnLatentActionTick> SleepTickDelegates;
    TMap<int32, FOnLatentActionCompleted> SleepCancelDelegates;

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="1.23", Keywords="sleep"))
    UPARAM(DisplayName = "UUID") int32 LaunchSleep_Internal(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo, FOnLatentActionTick TickDelegate, FOnLatentActionCompleted CompleteDelegate);

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", Keywords="sleep"))
    static void CancelSleep_Internal(UObject* WorldContextObject, int32 ActionUUID);

    UFUNCTION(BlueprintCallable, Category="Bango|Delay", meta = (WorldContext="WorldContextObject", Keywords="sleep"))
    static void SkipSleep_Internal(UObject* WorldContextObject, int32 ActionUUID);

    /*
    UFUNCTION(BlueprintCallable, Category="Bango|Logic", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="0.2", Keywords="sleep"))
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

    TMulticastDelegate<void()> OnComplete;
    TMulticastDelegate<void()> OnTick;

    bool bCancelled = false;
    bool bSkipped = false;
    
    FBangoSleepAction(float InDuration, const FLatentActionInfo& LatentInfo)
        : Duration(InDuration)
        , TimeRemaining(InDuration)
        , ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
    {
    }

    void UpdateOperation(FLatentResponse& Response) override
    {
        TimeRemaining -= Response.ElapsedTime();

        bool TimeFinished = TimeRemaining <= 0.0f || bSkipped;

        Response.FinishAndTriggerIf(TimeFinished || bCancelled, ExecutionFunction, OutputLink, CallbackTarget);

        if (TimeFinished)
        {
            if (!bCancelled)
            {
                OnComplete.Broadcast();
            }
        }
        else
        {
            OnTick.Broadcast();
        }
    }

    void NotifyActionAborted() override
    {
        bCancelled = true;
    }

    void Cancel()
    {
        bCancelled = true;
    }

    void Skip()
    {
        bSkipped = true;
    }

#if WITH_EDITOR
    // Returns a human readable description of the latent operation's current state
    virtual FString GetDescription() const override
    {
        static const FNumberFormattingOptions SleepTimeFormatOptions = FNumberFormattingOptions()
            .SetMinimumFractionalDigits(2)
            .SetMaximumFractionalDigits(2);
        return FText::Format(LOCTEXT("SleepActionTimeFmt", "{0} / {1}    "),
            FText::AsNumber(Duration - TimeRemaining, &SleepTimeFormatOptions),
            FText::AsNumber(Duration, &SleepTimeFormatOptions)).ToString();
    }
#endif
};

#undef LOCTEXT_NAMESPACE