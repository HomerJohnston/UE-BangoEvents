#pragma once

#include "EditorSubsystem.h"
#include "Engine/StreamableManager.h"

#include "BangoEditorSubsystem.generated.h"

class ABangoEvent;

UCLASS()
class UBangoEditorSubsystem : public UEditorSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
private:
	FStreamableManager StreamableManager;

public:
	FStreamableManager& GetStreamableManager()
	{
		return StreamableManager;
	}

	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void Tick(float DeltaTime) override;

	void OnBangoEventAltered(ABangoEvent* Event);
	
	void TryApplyMesh(ABangoEvent* Event);
	
	void ApplyLoadedMesh(ABangoEvent* Event, TSoftObjectPtr<UStaticMesh> LoadedObject);
	
	void TryApplyMaterial(ABangoEvent* Event);

	void ApplyLoadedMaterial(ABangoEvent* Event, TSoftObjectPtr<UMaterialInstance> LoadedMaterialInstance);
	
	TStatId GetStatId() const override;
};