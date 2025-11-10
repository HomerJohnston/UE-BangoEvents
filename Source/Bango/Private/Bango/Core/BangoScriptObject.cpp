#include "Bango/Core/BangoScriptObject.h"

#include "DelayAction.h"
#include "Bango/Core/BangoScriptHandle.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"

FBangoScriptHandle UBangoScriptObject::Execute_Internal()
{
    Handle = UBangoScriptSubsystem::RegisterScript(this);
    
    Start();

    return Handle;
}

void UBangoScriptObject::Finish(UBangoScriptObject* Script)
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

void UBangoScriptObject::Sleep(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        if (LatentActionManager.FindExistingAction<FBangoSleepAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
        {
            LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FBangoSleepAction(Duration, LatentInfo));
        }
    }
}

float UBangoScriptObject::Rand(float Hi, float Lo)
{
    return FMath::RandRange(Lo, Hi);
}
