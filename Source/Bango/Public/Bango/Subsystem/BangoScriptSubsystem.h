#pragma once
#include "Bango/Core/BangoScriptHandle.h"
#include "Bango/Utility/ObjectTicker.h"

#include "BangoScriptSubsystem.generated.h"

struct FBangoScriptHandle;
class UBangoScript;

UCLASS()
class UBangoScriptSubsystem : public UWorldSubsystem, public TObjectTicker<UBangoScriptSubsystem>
{
	GENERATED_BODY()

protected:
	static UBangoScriptSubsystem* Get(UObject* WorldContext);

protected:
	UPROPERTY()
	TMap<FBangoScriptHandle, TObjectPtr<UBangoScript>> RunningScripts;

	bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	static FBangoScriptHandle RegisterScript(UBangoScript* ScriptObject);
	
	static void UnregisterScript(UObject* WorldContext, FBangoScriptHandle& Handle);

	static void AbortScript(UObject* WorldContext, FBangoScriptHandle& Handle);
	
	void Tick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
};
