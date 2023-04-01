#pragma once

#include "DynamicMeshBuilder.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"

class UBangoPlungerComponent;

class FBangoPlungerSceneProxy final : public FPrimitiveSceneProxy
{
protected:
	FVector Origin;

	FLinearColor Color;

	float BoxSize = 30;
	float PlungerHalfHeight = 10;
	float PlungerHalfWidth = 15;
	float PlungerStemRadius = 2;
	float PlungerHandleRadius = 3;
	
	float Size = 30;
	bool bIsScreenSizeScaled = true;
	float ScreenSize = 0.0025;

private:
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;
	
	SIZE_T GetTypeHash() const override;

public:
	FBangoPlungerSceneProxy(UBangoPlungerComponent* Component);

	virtual ~FBangoPlungerSceneProxy();

	void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	
	void OnTransformChanged() override;

	uint32 GetMemoryFootprint() const override;

	uint32 GetAllocatedSize() const;
};
