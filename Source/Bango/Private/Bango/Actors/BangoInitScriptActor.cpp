#include "Bango/Actors/BangoInitScriptActor.h"

#include "Bango/Utility/BangoLog.h"
#include "Engine/Level.h"
#include "GameFramework/WorldSettings.h"

void ABangoInitScriptActor::BeginPlay()
{
#if !UE_BUILD_SHIPPING
	ULevel* Level = Cast<ULevel>(GetOuter());
	
	if (Level)
	{
		int32 WorldSettingsActorIndex = INDEX_NONE;
		int32 BangoInitScriptActorIndex = INDEX_NONE;
		
		// We'll just check the first 10ish actors. If we don't find it by now, it's probably not in the first actors...
		for (uint8 i = 0; i < 10; ++i)
		{
			if (!Level->Actors.IsValidIndex(i))
			{
				break;
			}
			
			AActor* Actor = Level->Actors[i];
			
			// This should be index 0
			if (Actor->IsA(AWorldSettings::StaticClass()))
			{
				WorldSettingsActorIndex = i;
			}
			else if (Actor->IsA(ABangoInitScriptActor::StaticClass()))
			{
				BangoInitScriptActorIndex = i;
			}
			
			if (WorldSettingsActorIndex >= 0 && BangoInitScriptActorIndex >= 0)
			{
				break;
			}
		}
		
		if (BangoInitScriptActorIndex >= 0 && (BangoInitScriptActorIndex - WorldSettingsActorIndex != 1))
		{
			UE_LOG(LogBango, Warning, TEXT("BangoInitScriptActor is not immediately after AWorldSettings! Did you drag a BangoInitScriptActor class into the level instead of using menus to add it?"));
		}
	}
#endif
	Super::BeginPlay();
}
