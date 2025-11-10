#pragma once
#include "Bango/Core/BangoScriptHandle.h"

#include "BangoScriptSubsystem.generated.h"

struct FBangoScriptHandle;
class UBangoScriptObject;

UCLASS()
class UBangoScriptSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	static UBangoScriptSubsystem* Get(UObject* WorldContext);

protected:
	UPROPERTY()
	TMap<FBangoScriptHandle, TObjectPtr<UBangoScriptObject>> RunningScripts;
	
public:
	static FBangoScriptHandle RegisterScript(UBangoScriptObject* BangoScriptObject);
	
	static void UnregisterScript(UObject* WorldContext, FBangoScriptHandle& Handle);

	static void AbortScript(UObject* WorldContext, FBangoScriptHandle& Handle);
};
