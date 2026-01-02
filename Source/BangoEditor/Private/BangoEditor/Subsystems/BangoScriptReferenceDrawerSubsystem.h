#pragma once

#include "BangoScriptReferenceDrawerSubsystem.generated.h"

UCLASS()
class UBangoScriptReferenceDrawerSubsystem : public UEditorSubsystem, public FTickableEditorObject
{
	GENERATED_BODY()
	
	void Tick(float DeltaTime) override;
	
	TStatId GetStatId() const override;
	
	bool IsTickable() const override;
};