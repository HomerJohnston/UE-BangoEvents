

#pragma once

#include "EditorSubsystem.h"
#include "Engine/Canvas.h"

#include "BangoDebugDrawService.generated.h"

UCLASS()
class UBangoDebugDrawService : public UEditorSubsystem
{
	GENERATED_BODY()

	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void DebugDraw(UCanvas* Canvas, APlayerController* ALWAYSNULL_DONOTUSE) const;
};
