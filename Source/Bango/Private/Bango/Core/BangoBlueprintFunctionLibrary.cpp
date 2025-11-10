// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Core/BangoBlueprintFunctionLibrary.h"

#include "Bango/Subsystem/BangoScriptSubsystem.h"

void UBangoBlueprintFunctionLibrary::AbortScript(UObject* WorldContext, FBangoScriptHandle& Handle)
{
	UBangoScriptSubsystem::AbortScript(WorldContext, Handle);

	Handle.Invalidate();
}
