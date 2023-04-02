// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bango.h"

#define LOCTEXT_NAMESPACE "FBangoModule"

namespace GhostPepperGames
{
	namespace Bango
	{
		extern bool bShowEventsInGame = false;
		static FAutoConsoleVariableRef CVarShowEventsInGame(TEXT("Bango.ShowEventsInGame"),
			bShowEventsInGame,
			TEXT("Displays event debug info during Play In Editor"));
	}
}

void FBangoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBangoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBangoModule, Bango);
