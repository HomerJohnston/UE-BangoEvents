#include "BangoScripts/Core/BangoScriptContainer.h"

#include "BangoScripts/Core/BangoScript.h"
#include "BangoScripts/Utility/BangoScriptsLog.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

// ----------------------------------------------

FSoftObjectPath FBangoScriptContainer::GetSanitizedScriptClass() const
{
	FString Path = ScriptClass.ToSoftObjectPath().ToString();
	
#if WITH_EDITOR
	int32 PIEInstanceID = INDEX_NONE;
	
	UWorld* World = GEditor->PlayWorld;

	if (FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(World))
	{
		PIEInstanceID = WorldContext->PIEInstance;
	}
	
	if (PIEInstanceID != INDEX_NONE)
	{
		if (FPackageName::GetLongPackageAssetName(Path).StartsWith(PLAYWORLD_PACKAGE_PREFIX))
		{
			FString PIEPrefix = FString::Printf(TEXT("%s_%d_"), PLAYWORLD_PACKAGE_PREFIX, PIEInstanceID);
			Path = Path.Replace(*PIEPrefix, TEXT(""), ESearchCase::CaseSensitive);
		}
	}
#endif
	
	return FSoftObjectPath{Path};
}

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::SetGuid(const FGuid& InGuid)
{
	check(!Guid.IsValid());
	
	Guid = InGuid;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::Unset()
{
	Guid.Invalidate();
	ScriptClass = nullptr;
	ScriptClass.Reset();
}
#endif
