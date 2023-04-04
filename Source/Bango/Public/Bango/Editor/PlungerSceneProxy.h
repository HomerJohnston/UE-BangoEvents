#pragma once

#include "DynamicMeshBuilder.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"

struct FBangoEventStateFlag;

class UBangoPlungerComponent;

class FBangoPlungerSceneProxy final : public FPrimitiveSceneProxy
{
	// CONSTRUCTION
	// ============================================================================================
public:
	FBangoPlungerSceneProxy(UBangoPlungerComponent* OwnerComponent);

	virtual ~FBangoPlungerSceneProxy();
	
	// SETTINGS
	// ============================================================================================

	const float PlungerBoxSize = 30;
	
	const float PlungerStemLength = 20;
	const float PlungerStemRadius = 2;
	const float PlungerHandleLength = 20;
	const float PlungerHandleRadius = 3;
	
	const float HandleOffsetUp = 0.0f;
	const float HandleOffsetDown = -10.0f;
	
	// SETTINGS GETTERS AND SETTERS
	// ------------------------------------------
	
	// STATE
	// ============================================================================================
private:
	TWeakObjectPtr<UBangoPlungerComponent> Component;
	
	FVector Origin = FVector::ZeroVector;

	FLinearColor DesiredColor = FLinearColor::White;

	FDynamicMeshIndexBuffer32 IndexBuffer_HandleUp;
	FStaticMeshVertexBuffers VertexBuffers_HandleUp;
	FLocalVertexFactory VertexFactory_HandleUp;

	FDynamicMeshIndexBuffer32 IndexBuffer_HandleDown;
	FStaticMeshVertexBuffers VertexBuffers_HandleDown;
	FLocalVertexFactory VertexFactory_HandleDown;
	
	// STATE GETTERS AND SETTERS
	// ------------------------------------------
public:
	/**  */

	// API
	// ============================================================================================
public:	
	SIZE_T GetTypeHash() const override;
	
	void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	
	void OnTransformChanged() override;

	uint32 GetMemoryFootprint() const override;
};
