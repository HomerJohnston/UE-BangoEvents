#pragma once
#include "EditorSubsystem.h"
#include "Bango/Components/BangoScriptComponent.h"

#include "BangoEditorScriptClickSubsystem.generated.h"

UCLASS()
class UBangoEditorScriptClickSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void Deinitialize() override;
	
	void OnBangoScriptComponentClicked(UBangoScriptComponent* Component) const;
};
