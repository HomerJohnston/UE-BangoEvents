// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Module/Bango.h"

#define LOCTEXT_NAMESPACE "FBangoModule"

void FBangoModule::StartupModule()
{
}

void FBangoModule::ShutdownModule()
{
}

ASomeTestActor::ASomeTestActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshVisibleTest");
	//MeshComp->SetVisibility(false);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBangoModule, Bango);
