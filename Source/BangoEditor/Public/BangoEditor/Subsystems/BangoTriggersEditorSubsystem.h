#pragma once

#include "BangoTriggersEditorSubsystem.generated.h"

class UBangoEditorWorldExtension;

UCLASS()
class UBangoTriggersEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues InitValues);
	
	UPROPERTY()
	TObjectPtr<UBangoEditorWorldExtension> WorldExtension;
};