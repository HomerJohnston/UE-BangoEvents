#pragma once

#include "EditorSubsystem.h"

#include "BangoActorIDEditorSubsystem.generated.h"

/**
 * This subsystem is responsible for managing Actor ID components. 
 */
UCLASS()
class UBangoActorIDEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void Deinitialize() override;
	
	void OnRequestNewID(AActor* Actor) const;
};