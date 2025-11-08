#include "Bango/K2/BangoWaitForAnyNode.h"

void UK2Node_BangoWaitForAny::Activate()
{
    WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::OnRunComplete);
}

UK2Node_BangoWaitForAny* UK2Node_BangoWaitForAny::Run(const UObject* WorldContextObject, float abc)
{
    UK2Node_BangoWaitForAny* Node = NewObject<UK2Node_BangoWaitForAny>();
    Node->RegisterWithGameInstance((UObject*)nullptr);
    
    return Node;
}

void UK2Node_BangoWaitForAny::OnRunComplete()
{
    SetReadyToDestroy();
}


UDelayOneFrame::UDelayOneFrame(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), WorldContextObject(nullptr), MyFloatInput(0.0f)
{
}

UDelayOneFrame* UDelayOneFrame::WaitForOneFrame(const UObject* WorldContextObject, const float SomeInputVariables)
{
    UDelayOneFrame* BlueprintNode = NewObject<UDelayOneFrame>();
    BlueprintNode->WorldContextObject = WorldContextObject;
    BlueprintNode->MyFloatInput = SomeInputVariables;
    return BlueprintNode;
}

void UDelayOneFrame::Activate()
{
    // Any safety checks should be performed here. Check here validity of all your pointers etc.
    // You can log any errors using FFrame::KismetExecutionMessage, like that:
    // FFrame::KismetExecutionMessage(TEXT("Valid Player Controller reference is needed for ... to start!"), ELogVerbosity::Error);
    // return;

    WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UDelayOneFrame::ExecuteAfterOneFrame);
}

void UDelayOneFrame::ExecuteAfterOneFrame()
{
    AfterOneFrame.Broadcast(MyFloatInput + 1.0f, MyFloatInput + 2.0f);
    // remember to unregister with the game instance so it can be
    // garbage collected now that we're done
    SetReadyToDestroy();
}