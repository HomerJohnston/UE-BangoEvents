// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Bango.h"

#include "Bango/CVars.h"

#define LOCTEXT_NAMESPACE "FBangoModule"


void FBangoModule::StartupModule()
{
}

void FBangoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBangoModule, Bango);
