#include "Bango/Subsystem/BangoScriptSubsystem.h"

#include "Bango/Core/BangoScriptHandle.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Bango/Utility/BangoLog.h"

UBangoScriptSubsystem* UBangoScriptSubsystem::Get(UObject* WorldContext)
{
	if (IsValid(WorldContext))
	{
		return WorldContext->GetWorld()->GetSubsystem<UBangoScriptSubsystem>();
	}

	return nullptr;
}

bool UBangoScriptSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UBangoScriptSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TickFunction.RegisterTickFunction(GetWorld()->PersistentLevel);
}

FBangoScriptHandle UBangoScriptSubsystem::RegisterScript(UBangoScriptObject* ScriptObject)
{
	UBangoScriptSubsystem* Subsystem = Get(ScriptObject);

	FBangoScriptHandle NewHandle = FBangoScriptHandle();
	
	Subsystem->RunningScripts.Add(NewHandle, ScriptObject);

	UE_LOG(LogBango, Display, TEXT("Script running: {%s}"), *ScriptObject->GetName());
	
	return NewHandle;
}

void UBangoScriptSubsystem::UnregisterScript(UObject* WorldContext, FBangoScriptHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}
	
	UBangoScriptSubsystem* Subsystem = Get(WorldContext);

	TObjectPtr<UBangoScriptObject> Script;
	
	if (Subsystem->RunningScripts.RemoveAndCopyValue(Handle, Script))
	{
		UE_LOG(LogBango, Display, TEXT("Script halting: {%s}"), *Script->GetName());
		UBangoScriptObject::Finish(Script);
	}

	Handle.Invalidate();
}

void UBangoScriptSubsystem::AbortScript(UObject* WorldContext, FBangoScriptHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}
	
	UnregisterScript(WorldContext, Handle);

	Handle.Invalidate();
}

void UBangoScriptSubsystem::Tick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	FLatentActionManager& Manager = World->GetLatentActionManager();

	TArray<FBangoScriptHandle> DeadScriptHandles;
	
	for (auto [Handle, Script] : RunningScripts)
	{
		TSet<int32> UUIDSet;
		Manager.GetActiveUUIDs(Script, /*out*/ UUIDSet);
		
		if (UUIDSet.Num() == 0)
		{
			UE_LOG(LogBango, Display, TEXT("Idle script being automatically destroyed: {%s}"), *Script->GetName());
			DeadScriptHandles.Add(Handle);
		}
	}
	
	for (FBangoScriptHandle& Handle : DeadScriptHandles)
	{
		UnregisterScript(this, Handle);
	}
}
