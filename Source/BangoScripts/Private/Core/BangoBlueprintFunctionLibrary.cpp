// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "BangoScripts/Core/BangoBlueprintFunctionLibrary.h"

#include "BangoScripts/Subsystem/BangoScriptSubsystem.h"

void UBangoBlueprintFunctionLibrary::AbortScript(UObject* WorldContext, FBangoScriptHandle& Handle)
{
	UBangoScriptSubsystem::AbortScript(WorldContext, Handle);

	Handle.Invalidate();
}
