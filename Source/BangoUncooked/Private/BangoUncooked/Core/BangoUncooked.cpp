#include "BangoUncooked/Core/BangoUncooked.h"

#include "BlueprintCompilationManager.h"
#include "BlueprintCompilerExtension.h"

#define LOCTEXT_NAMESPACE "FBangoUncookedModule"

void FBangoUncookedModule::StartupModule()
{

}

void FBangoUncookedModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBangoUncookedModule, BangoUncooked)