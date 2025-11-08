// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoScriptObject.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FWaitUntilDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnFinishDelegate);

UCLASS(Abstract, Blueprintable)
class UBangoScriptObject : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) // TODO this should not be BP callable in the end
    void Execute();

    void Execute_Implementation() {};

    bool ImplementsGetWorld() const override { return true; }

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void Finish(UObject* WorldContextObject);

    TMulticastDelegate<void()> OnFinishDelegate;
    
    /** 
     * Perform a latent action with a delay (specified in seconds).  Calling again while it is counting down will be ignored.
     * 
     * @param WorldContext	World context.
     * @param Duration 		length of delay (in seconds).
     * @param LatentInfo 	The latent action.
     */
    UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="0.2", Keywords="sleep"))
    static void Sleep(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo );

    UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", Duration="0.2", Keywords="sleep"))
    static void WaitUntil(bool bCondition) {};

    UFUNCTION(BlueprintCallable, Category="Bango|Utilities", BlueprintPure)
    static UPARAM(DisplayName="Val") float Rand(float Hi, float Lo);
};

class FBangoDelayAction : public FPendingLatentAction
{
public:
    float Duration;
    float TimeRemaining;
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    FBangoDelayAction(float InDuration, const FLatentActionInfo& LatentInfo)
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

#if WITH_EDITOR
    // Returns a human readable description of the latent operation's current state
    virtual FString GetDescription() const override
    {
        static const FNumberFormattingOptions DelayTimeFormatOptions = FNumberFormattingOptions()
            .SetMinimumFractionalDigits(3)
            .SetMaximumFractionalDigits(3);
        return FText::Format(NSLOCTEXT("DelayAction", "DelayActionTimeFmt", "{0}"), FText::AsNumber(TimeRemaining / Duration, &DelayTimeFormatOptions)).ToString();
    }
#endif
};