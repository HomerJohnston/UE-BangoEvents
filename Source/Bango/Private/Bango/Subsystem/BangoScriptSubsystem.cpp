#include "Bango/Subsystem/BangoScriptSubsystem.h"

#include "Bango/Core/BangoScriptHandle.h"
#include "Bango/Core/BangoScriptObject.h"

UBangoScriptSubsystem* UBangoScriptSubsystem::Get(UObject* WorldContext)
{
	if (IsValid(WorldContext))
	{
		return WorldContext->GetWorld()->GetSubsystem<UBangoScriptSubsystem>();
	}

	return nullptr;
}

FBangoScriptHandle UBangoScriptSubsystem::RegisterScript(UBangoScriptObject* BangoScriptObject)
{
	UBangoScriptSubsystem* Subsystem = Get(BangoScriptObject);

	FBangoScriptHandle NewHandle = FBangoScriptHandle();
	
	Subsystem->RunningScripts.Add(NewHandle, BangoScriptObject);

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
