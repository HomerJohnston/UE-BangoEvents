#pragma once

#include "DynamicMeshBuilder.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"

class UBangoPlungerComponent;

class FBangoPlungerSceneProxy final : public FPrimitiveSceneProxy
{
	// CONSTRUCTION
	// ============================================================================================
public:
	FBangoPlungerSceneProxy(UBangoPlungerComponent* Component);

	virtual ~FBangoPlungerSceneProxy();
	
	// SETTINGS
	// ============================================================================================
public:
	/**  */
	const float PlungerBoxSize = 30;
	
	/**  */
	const float PlungerStemHeight = 10;
	
	/**  */
	const float PlungerHalfWidth = 15;
	
	/**  */
	const float PlungerStemRadius = 2;

	/**  */
	const float PlungerHandleWidth = 20;
	
	/**  */
	const float PlungerHandleRadius = 3;
	
	/**  */
	const float Size = 30;
	
	/**  */
	const float ScreenSize = 0.0025;
	
	/**  */
	const bool bIsScreenSizeScaled = true;

	/**  */
	const double RecentPushColorCooldownTime = 1.0;
	
	// SETTINGS GETTERS AND SETTERS
	// ------------------------------------------
	
	// STATE
	// ============================================================================================
private:
	FVector Origin = FVector::ZeroVector;

	FLinearColor Color = FLinearColor::White;

	bool bPlungerPushed = false;

	double LastPushTime = DBL_MIN;

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
	void SetPlungerPushed(bool bNewPushedState);

	/** */
	void SetColor(FLinearColor NewColor);
	
	// API
	// ============================================================================================
public:	
	SIZE_T GetTypeHash() const override;
	
	void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	
	void OnTransformChanged() override;

	uint32 GetMemoryFootprint() const override;
};
