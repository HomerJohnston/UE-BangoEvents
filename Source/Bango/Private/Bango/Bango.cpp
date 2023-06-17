// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Bango.h"

#include "Bango/CVars.h"

#define LOCTEXT_NAMESPACE "FBangoModule"


void FBangoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IConsoleVariable* ShowEventsInGameCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));

	ShowEventsInGameCVar->Set(true);

	GhostPepperGames::Bango::
}

void FBangoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBangoModule, Bango);
