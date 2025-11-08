#include "Bango/Core/BangoScriptObject.h"

#include "DelayAction.h"

void UBangoScriptObject::Sleep(const UObject* WorldContextObject, float Duration, struct FLatentActionInfo LatentInfo)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        if (LatentActionManager.FindExistingAction<FBangoDelayAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
        {
            LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FBangoDelayAction(Duration, LatentInfo));
        }
    }
}

float UBangoScriptObject::Rand(float Hi, float Lo)
{
    return FMath::RandRange(Lo, Hi);
}

void UBangoScriptObject::Finish(UObject* WorldContextObject)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

        LatentActionManager.RemoveActionsForObject(WorldContextObject);
    }

    OnFinishDelegate.Broadcast();
}
