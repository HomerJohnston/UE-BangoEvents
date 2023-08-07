// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "DynamicMeshBuilder.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"

struct FPunyEventStateFlag;
struct FPunyPlungerDynamicData;
class UPunyPlungerComponent;

struct FPlungerMeshConstructionData
{
	FLocalVertexFactory* VertexFactory;
	FStaticMeshVertexBuffers* VertexBuffer;
	FDynamicMeshIndexBuffer32* IndexBuffer;
	
	float BoxSize;

	float StemHeight;
	float StemRadius;
	
	float HandleRadius;
	float HandleWidth;
	
	float StemAndHandleZOffset;
};

class FPunyPlungerSceneProxy final : public FPrimitiveSceneProxy
{
	// CONSTRUCTION
	// ============================================================================================
public:
	FPunyPlungerSceneProxy(UPunyPlungerComponent* OwnerComponent);

	virtual ~FPunyPlungerSceneProxy();
	
	// SETTINGS
	// ============================================================================================

	const float PlungerBoxSize = 30;
	
	const float PlungerStemLength = 20;
	const float PlungerStemRadius = 2;
	const float PlungerHandleLength = 20;
	const float PlungerHandleRadius = 3;
	
	const float HandleOffsetUp = 0.0f;
	const float HandleOffsetDown = -10.0f;
	
	const bool bIsScreenSizeScaled;
	
	const float ScreenSize;
	
	// SETTINGS GETTERS AND SETTERS
	// ------------------------------------------
	
	// STATE
	// ============================================================================================
private:
	FVector Origin = FVector::ZeroVector;

	FLinearColor DesiredColor = FLinearColor::White;

	FDynamicMeshIndexBuffer32 IndexBuffer_HandleUp;
	FStaticMeshVertexBuffers VertexBuffers_HandleUp;
	FLocalVertexFactory VertexFactory_HandleUp;

	FDynamicMeshIndexBuffer32 IndexBuffer_HandleDown;
	FStaticMeshVertexBuffers VertexBuffers_HandleDown;
	FLocalVertexFactory VertexFactory_HandleDown;

private:
	FLinearColor ColorState;
	bool bPushState = false;
	
	// STATE GETTERS AND SETTERS
	// ------------------------------------------
public:
	/**  */
	void SetColorState(FLinearColor& NewColorState);

	void SetPushState(bool bNewPushState);
	
	// API
	// ============================================================================================
public:	
	SIZE_T GetTypeHash() const override;
	
	void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	
	void OnTransformChanged() override;

	uint32 GetMemoryFootprint() const override;

	void SetDynamicData_RenderThread(FPunyPlungerDynamicData* NewDynamicData);
	
private:
	void PreparePlungerMesh(const FPlungerMeshConstructionData& MeshData);
};
