#include "Bango/Core/BangoScriptObject.h"

#include "DelayAction.h"
#include "Bango/Core/BangoScriptHandle.h"
#include "Bango/LatentActions/BangoSleepAction.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "Bango/Utility/BangoLog.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "Bango"

#if WITH_EDITOR
DataValidationDelegate UBangoScriptInstance::OnScriptRequestValidation;
#endif

void UBangoScriptInstance::RunScript(TSubclassOf<UBangoScriptInstance> Script, UObject* Runner, UObject* WorldContext)
{
	if (WorldContext == nullptr)
	{
		WorldContext = Runner;
	}
	
	if (!WorldContext)
	{
		UE_LOG(LogBango, Error, TEXT("Tried to launch script but Runner and WorldContext were null!"));
		return;
	}
	
	// TODO should I bother pooling?
	UBangoScriptInstance* NewScriptInstance = NewObject<UBangoScriptInstance>(Runner, Script);
	NewScriptInstance->Execute_Internal();
}

FBangoScriptHandle UBangoScriptInstance::Execute_Internal()
{
    Handle = UBangoScriptSubsystem::RegisterScript(this);
    Start();
    return Handle;
}

void UBangoScriptInstance::Finish(UBangoScriptInstance* Script)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(Script, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

        LatentActionManager.RemoveActionsForObject(Script);
    }
    
    UBangoScriptSubsystem::UnregisterScript(Script, Script->Handle);

    Script->OnFinishDelegate.Broadcast();

    Script->Handle.Invalidate();
}

int32 UBangoScriptInstance::LaunchSleep_Internal(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo, FOnLatentActionTick BPDelayTickEvent, FOnLatentActionCompleted BPDelayCompleteEvent)
{
    int32 UUID = LatentInfo.UUID;
    
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        if (!LatentActionManager.FindExistingAction<FBangoSleepAction>(LatentInfo.CallbackTarget, UUID))
        {
            FBangoSleepAction* SleepAction = new FBangoSleepAction(Duration, LatentInfo);
            LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, UUID, SleepAction);

            FOnLatentActionTick TickDelegate;
            TickDelegate = BPDelayTickEvent;
            
            SleepAction->OnTick.AddLambda([TickDelegate]()
            {
                TickDelegate.ExecuteIfBound();
            });

            FOnLatentActionCompleted CompleteDelegate;
            CompleteDelegate = BPDelayCompleteEvent;
            
            SleepAction->OnComplete.AddLambda([CompleteDelegate]()
            {
                CompleteDelegate.ExecuteIfBound();
            });

            return UUID;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Unknown error running LaunchSleep_Internal!"));
    return 0;
}

void UBangoScriptInstance::CancelSleep_Internal(UObject* WorldContextObject, int32 ActionUUID)
{
    if (ActionUUID == 0)
    {
        UE_LOG(LogBango, Warning, TEXT("CancelSleep_Internal called with ActionUUID {%i}"), ActionUUID);
        return;
    }
    
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        FBangoSleepAction* Action = LatentActionManager.FindExistingAction<FBangoSleepAction>(WorldContextObject, ActionUUID);

        if (Action)
        {
            Action->Cancel();
        }
    }
}

void UBangoScriptInstance::SkipSleep_Internal(UObject* WorldContextObject, int32 ActionUUID)
{
    if (ActionUUID == 0)
    {
        UE_LOG(LogBango, Warning, TEXT("SkipSleep_Internal called with ActionUUID {%i}"), ActionUUID);
        return;
    }
    
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        FBangoSleepAction* Action = LatentActionManager.FindExistingAction<FBangoSleepAction>(WorldContextObject, ActionUUID);

        if (Action)
        {
            Action->Skip();
        }
    }    
}

void UBangoScriptInstance::SetSleepPause_Internal(UObject* WorldContextObject, bool bPaused, int32 ActionUUID)
{
    if (ActionUUID == 0)
    {
        UE_LOG(LogBango, Warning, TEXT("SetSleepPause_Internal called with ActionUUID {%i}"), ActionUUID);
        return;
    }
    
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        FBangoSleepAction* Action = LatentActionManager.FindExistingAction<FBangoSleepAction>(WorldContextObject, ActionUUID);

        if (Action)
        {
            Action->SetPaused(bPaused);
        }
    }    
}

float UBangoScriptInstance::Rand(float Hi, float Lo)
{
    return FMath::RandRange(Lo, Hi);
}

#if WITH_EDITOR
EDataValidationResult UBangoScriptInstance::IsDataValid(class FDataValidationContext& Context) const
{
    if (OnScriptRequestValidation.IsBound())
    {
        return OnScriptRequestValidation.Execute(Context, this);        
    }
    
    return UObject::IsDataValid(Context);
}

void UBangoScriptInstance::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	// TODO API update
	// BangoScriptObject.cpp(171,12): Warning C4996 : 'UObject::GetAssetRegistryTags': Implement the version that takes FAssetRegistryTagsContext instead. - Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.
    Super::GetAssetRegistryTags(OutTags);
    
    FAssetRegistryTag Tag(TEXT("Test"), TEXT("TesT"), FAssetRegistryTag::TT_Alphabetical, FAssetRegistryTag::TD_None);
    
    OutTags.Add(Tag);
}
#endif

#undef LOCTEXT_NAMESPACE