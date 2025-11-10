#include "Bango/K2/BangoRunScriptNode.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Bango/Core/BangoScriptObject.h"

void UBangoRunScript::Activate()
{
    UE_LOG(LogTemp, Display, TEXT("HEllo WOrld "));
}

void UBangoRunScript::Cancel()
{
    Super::Cancel();

    UE_LOG(LogTemp, Display, TEXT("HEllo WOrld Cancel"));
}

void UBangoRunScript::SetReadyToDestroy()
{
    Super::SetReadyToDestroy();
}
