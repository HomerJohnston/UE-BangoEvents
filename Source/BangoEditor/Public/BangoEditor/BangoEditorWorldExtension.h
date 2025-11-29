#pragma once

#include "EditorWorldExtension.h"

#include "BangoEditorWorldExtension.generated.h"

UCLASS()
class UBangoEditorWorldExtension : public UEditorWorldExtension
{
	GENERATED_BODY()
	
public:
	void Init() override;
	
	bool InputKey(FEditorViewportClient* InViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
};