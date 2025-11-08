// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Bango.generated.h"

class FBangoModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

UCLASS(Blueprintable)
class ASomeTestActor : public AActor
{
	GENERATED_BODY()

public:

	ASomeTestActor();
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;
};