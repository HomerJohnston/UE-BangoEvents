#include "Bango/Core/BangoScript.h"

#include "DelayAction.h"
#include "Bango/Core/BangoScriptHandle.h"
#include "Bango/LatentActions/BangoSleepAction.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "Bango/Utility/BangoLog.h"
#include "Misc/DataValidation.h"
#include "UObject/AssetRegistryTagsContext.h"

#define LOCTEXT_NAMESPACE "Bango"

#if WITH_EDITOR
DataValidationDelegate UBangoScript::OnScriptRequestValidation;
FSoftClassPath UBangoScript::SelectedScript;
#endif

/*
FBangoScriptHandle UBangoScript::RunScript(TSubclassOf<UBangoScript> Script, UObject* Runner, UObject* WorldContext)
{
	if (WorldContext == nullptr)
	{
		WorldContext = Runner;
	}
	
	if (!Runner)
	{
		UE_LOG(LogBango, Error, TEXT("RunScript called with null runner!"));
		return FBangoScriptHandle::GetNullHandle();
	}
	
	if (!Script)
	{
		UE_LOG(LogBango, Warning, TEXT("RunScript called with null script!"));
		return FBangoScriptHandle::GetNullHandle();
	}
	
	if (!WorldContext)
	{
		UE_LOG(LogBango, Error, TEXT("Tried to launch script but Runner and WorldContext were null!"));
		return FBangoScriptHandle::GetNullHandle();
	}
	
	// TODO should I implement pooling? Maybe optional?
 	UBangoScript* NewScriptInstance = NewObject<UBangoScript>(Runner, Script);
	NewScriptInstance->This = Runner;
	
	return NewScriptInstance->Execute_Internal();
}
*/

/*
FBangoScriptHandle UBangoScript::RunScript(TSoftClassPtr<UBangoScript> Script, UObject* Runner, UObject* WorldContext)
{
	TSoftClassPtr<UBangoScript> ScriptClassReal = TSoftClassPtr<UBangoScript>( GetSanitizedScriptClass() );
	
	if (ScriptClassReal.IsValid())
	{
		return UBangoScript::RunScript(ScriptClassReal.Get(), Runner);
	}
	else if (bImmediate)
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		TSubclassOf<UBangoScript> LoadedScriptClass = Streamable.LoadSynchronous(ScriptClassReal);
		
		UE_LOG(LogBango, Warning, TEXT("Synchronously loading Bango script"));
		
		return UBangoScript::RunScript(LoadedScriptClass, Runner);
	}
	else 
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		
		TSoftClassPtr<UBangoScript> ScriptClassIn = ScriptClassReal;
		
		auto RunOnLoad = FStreamableDelegate::CreateWeakLambda(Runner, [Runner, ScriptClassIn] ()
		{
			UBangoScript::RunScript(ScriptClassIn.LoadSynchronous(), Runner);
		});
		
		// TODO I am not stashing the StreamableHandle because I am running the script immediately on load, upon whence the script subsystem will keep it alive - does this cause race issues?
		Streamable.RequestAsyncLoad(ScriptClassReal.ToSoftObjectPath(), RunOnLoad);
	}
}
*/

void UBangoScript::Finish(UBangoScript* Script)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(Script, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

        LatentActionManager.RemoveActionsForObject(Script);
    }
    
	Script->OnFinish_Native.Broadcast(Script->Handle);
    Script->OnFinishDelegate.Broadcast();
    Script->Handle.Invalidate();
	
	Script->MarkAsGarbage();
}

int32 UBangoScript::LaunchSleep_Internal(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo, FOnLatentActionTick BPDelayTickEvent, FOnLatentActionCompleted BPDelayCompleteEvent)
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

    UE_LOG(LogBango, Warning, TEXT("Unknown error running LaunchSleep_Internal!"));
    return 0;
}

void UBangoScript::CancelSleep_Internal(UObject* WorldContextObject, int32 ActionUUID)
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

void UBangoScript::SkipSleep_Internal(UObject* WorldContextObject, int32 ActionUUID)
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

void UBangoScript::SetSleepPause_Internal(UObject* WorldContextObject, bool bPaused, int32 ActionUUID)
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

float UBangoScript::Rand(float Hi, float Lo)
{
    return FMath::RandRange(Lo, Hi);
}

#if WITH_EDITOR
EDataValidationResult UBangoScript::IsDataValid(class FDataValidationContext& Context) const
{
    if (OnScriptRequestValidation.IsBound())
    {
        return OnScriptRequestValidation.Execute(Context, this);        
    }
    
    return UObject::IsDataValid(Context);
}
#endif

/*
void UBangoScript::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	// TODO API update
	// BangoScriptObject.cpp(171,12): Warning C4996 : 'UObject::GetAssetRegistryTags': Implement the version that takes FAssetRegistryTagsContext instead. - Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.
	FAssetRegistryTagsContext Context()
    Super::GetAssetRegistryTags(OutTags);
    
    FAssetRegistryTag Tag(TEXT("Test"), TEXT("TesT"), FAssetRegistryTag::TT_Alphabetical, FAssetRegistryTag::TD_None);
    
    OutTags.Add(Tag);
}
*/

#undef LOCTEXT_NAMESPACE