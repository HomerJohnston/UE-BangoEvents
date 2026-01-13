#pragma once

#include "ComponentVisualizer.h"

class FBangoScriptComponentVisualizer : public FComponentVisualizer
{
	
	void OnRegister() override;
	
	void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	
	void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	
	void DrawScreenCircleOverWorldPos(const FSceneView* View, FCanvas* Canvas, const FVector& WorldPosition, float Radius, float Thickness, const FLinearColor& Color);
	
	void DrawScreenLineForWorldPos(const FSceneView* View, FCanvas* Canvas, const FVector& WorldStart, const FVector& WorldEnd, float Thickness, const FLinearColor& Color, float StartCutoff = 0.0f, float EndCutoff = 0.0f);
	
	bool GetActorScreenPosAndSize(const FSceneView* View, FCanvas* Canvas, const AActor* Actor, FVector& ScreenLocation, float& Radius);
	
	bool GetScreenPos(const FSceneView* View, const FVector& WorldPos, FVector2D& ScreenPos);
};
