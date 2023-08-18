// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "DynamicMeshBuilder.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"

struct FBangoEventStateFlag;
struct FBangoPlungerDynamicData;
class UBangoPlungerComponent;

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
	// Dynamic Data
	FLinearColor BaseColor = FLinearColor::Black;

	double ActivationTime = -1;
	double DeactivationTime = -1;

	bool bIsFrozen = false;
	bool bIsActive = false;
	bool bIsExpired = false;
	
	// STATE GETTERS AND SETTERS
	// ------------------------------------------
public:
	void SetDynamicData_RenderThread(FBangoPlungerDynamicData* NewDynamicData);

	// API
	// ============================================================================================
public:	
	SIZE_T GetTypeHash() const override;
	
	void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	
	void OnTransformChanged() override;

	uint32 GetMemoryFootprint() const override;

private:
	void PreparePlungerMesh(const FPlungerMeshConstructionData& MeshData);

	FLinearColor DetermineColor() const;
};
