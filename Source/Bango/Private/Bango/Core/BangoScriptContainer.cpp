#include "Bango/Core/BangoScriptContainer.h"

#include "Bango/Core/BangoScript.h"
#include "Bango/Utility/BangoLog.h"
#include "Engine/AssetManager.h"

/*
FBangoScriptHandle FBangoScriptContainer::Run(UObject* Runner, bool bImmediate)
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

#if WITH_EDITOR
void FBangoScriptContainer::SetGuid(const FGuid& InGuid)
{
	check(!Guid.IsValid());
	
	Guid = InGuid;
}

void FBangoScriptContainer::Unset()
{
	Guid.Invalidate();
	ScriptClass = nullptr;
	ScriptClass.Reset();
}

void FBangoScriptContainer::TryRestoreScriptFromTransientPackage()
{
}
#endif
