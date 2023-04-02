#include "Bango/Editor/PlungerSceneProxy.h"
#include "Bango/Editor/PlungerComponent.h"
#include "SceneManagement.h"

// Forward declarations
//=================================================================================================

void BuildBoxVerts(const FVector3f& StaticOffset, TArray<FDynamicMeshVertex>& OutVerts, TArray<uint32>& OutIndices);
void BuildCylinderVerts(const FVector& Base, const FVector& XAxis, const FVector& YAxis, const FVector& ZAxis, double Radius, double HalfHeight, uint32 Sides, TArray<FDynamicMeshVertex>& OutVerts, TArray<uint32>& OutIndices);


// FBangoPlungerSceneProxy
//=================================================================================================

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

void PreparePlungerMesh(const FPlungerMeshConstructionData& MeshData);

// CONSTRUCTION
// ------------------------------------------------------------------------------------------------
FBangoPlungerSceneProxy::FBangoPlungerSceneProxy(UBangoPlungerComponent* Component)
	: FPrimitiveSceneProxy(Component),
	Color(FColor::Turquoise),
	VertexFactory_HandleUp(GetScene().GetFeatureLevel(), "FBangoPlungerSceneProxy"),
	VertexFactory_HandleDown(GetScene().GetFeatureLevel(), "FBangoPlungerSceneProxy")
{
	bWillEverBeLit = false;

	// Mesh Common
	const float HandleOffsetUp = 0.0f;
	const float HandleOffsetDown = -40.0f;

	FPlungerMeshConstructionData MeshData;

	MeshData.BoxSize = PlungerBoxSize;
	
	MeshData.StemHeight = PlungerStemHeight;
	MeshData.StemRadius = PlungerStemRadius;

	MeshData.HandleRadius = PlungerHandleRadius;
	MeshData.HandleWidth = PlungerHandleWidth;

	{
		MeshData.VertexFactory = &VertexFactory_HandleDown;
		MeshData.VertexBuffer = &VertexBuffers_HandleDown;
		MeshData.IndexBuffer = &IndexBuffer_HandleDown;
		MeshData.StemAndHandleZOffset = HandleOffsetDown;
	}
	PreparePlungerMesh(MeshData);

	{
		MeshData.VertexFactory = &VertexFactory_HandleUp;
		MeshData.VertexBuffer = &VertexBuffers_HandleUp;
		MeshData.IndexBuffer = &IndexBuffer_HandleUp;
		MeshData.StemAndHandleZOffset = HandleOffsetUp;
	}
	PreparePlungerMesh(MeshData);
}

void PreparePlungerMesh(const FPlungerMeshConstructionData& MeshData)
{
	TArray<FDynamicMeshVertex> OutVerts_HandleUp;

	FVector3f BoxOrigin(0.0f, 0.0f, 0.5f * MeshData.BoxSize);
	
	FVector StemOffset(0, 0, MeshData.BoxSize + 0.5f * MeshData.StemHeight + MeshData.StemAndHandleZOffset);
	FVector HandleOffset(0, 0, StemOffset.Z + 0.5f * MeshData.StemHeight);

	BuildBoxVerts(BoxOrigin, OutVerts_HandleUp, MeshData.IndexBuffer->Indices);
	BuildCylinderVerts(StemOffset, FVector::ForwardVector, FVector::RightVector, FVector::UpVector, MeshData.StemRadius, 0.5f * MeshData.StemHeight, 8, OutVerts_HandleUp, MeshData.IndexBuffer->Indices);
	BuildCylinderVerts(HandleOffset, FVector::ForwardVector, FVector::UpVector, FVector::LeftVector, MeshData.HandleRadius, 0.5f * MeshData.HandleWidth, 8, OutVerts_HandleUp, MeshData.IndexBuffer->Indices);

	MeshData.VertexBuffer->InitFromDynamicVertex(MeshData.VertexFactory, OutVerts_HandleUp);

	BeginInitResource(MeshData.IndexBuffer);
}

FBangoPlungerSceneProxy::~FBangoPlungerSceneProxy()
{
	VertexBuffers_HandleUp.PositionVertexBuffer.ReleaseResource();
	VertexBuffers_HandleUp.StaticMeshVertexBuffer.ReleaseResource();
	VertexBuffers_HandleUp.ColorVertexBuffer.ReleaseResource();
	
	VertexBuffers_HandleDown.PositionVertexBuffer.ReleaseResource();
	VertexBuffers_HandleDown.StaticMeshVertexBuffer.ReleaseResource();
	VertexBuffers_HandleDown.ColorVertexBuffer.ReleaseResource();

	IndexBuffer_HandleUp.ReleaseResource();
	VertexFactory_HandleUp.ReleaseResource();
	
	IndexBuffer_HandleDown.ReleaseResource();
	VertexFactory_HandleDown.ReleaseResource();
}

// STATE GETTERS AND SETTERS
// ------------------------------------------------------------------------------------------------
void FBangoPlungerSceneProxy::SetPlungerPushed(bool bNewPushedState)
{
	bPlungerPushed = bNewPushedState;

	// TODO is there a safer way to do this?
	if (IsValid(GWorld))
	{
		LastPushTime = GWorld->GetTimeSeconds();
	}
}

void FBangoPlungerSceneProxy::SetColor(FLinearColor NewColor)
{
	Color = NewColor;
}

SIZE_T FBangoPlungerSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}


void FBangoPlungerSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	auto MaterialRenderProxy = new FColoredMaterialRenderProxy(GEngine->ArrowMaterial->GetRenderProxy(), Color, "GizmoColor");

	Collector.RegisterOneFrameMaterialProxy(MaterialRenderProxy);

	FMatrix EffLocalToWorld = GetLocalToWorld();
	
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

			const FSceneView* View = Views[ViewIndex];

			float ViewScale = 1.0f;

			if (bIsScreenSizeScaled && (View->ViewMatrices.GetProjectionMatrix().M[3][3] != 1.0f))
			{
				const float ZoomFactor = FMath::Min<float>(View->ViewMatrices.GetProjectionMatrix().M[0][0], View->ViewMatrices.GetProjectionMatrix().M[1][1]);

				if (ZoomFactor != 0.0f)
				{
					const float Radius = FMath::Abs(View->WorldToScreen(Origin).W * (ScreenSize / ZoomFactor));

					if (Radius < 1.0f)
					{
						ViewScale *= Radius;
					}
				}
			}

			FMeshBatch& Mesh = Collector.AllocateMesh();
			FMeshBatchElement& BatchElement = Mesh.Elements[0];

			const FLocalVertexFactory& VertexFactory = (bPlungerPushed) ? VertexFactory_HandleDown : VertexFactory_HandleUp;
			const FIndexBuffer& IndexBuffer = (bPlungerPushed) ? IndexBuffer_HandleDown : IndexBuffer_HandleUp;
			
			BatchElement.IndexBuffer = &IndexBuffer;
			
			Mesh.bWireframe = false;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = MaterialRenderProxy;

			auto& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
			DynamicPrimitiveUniformBuffer.Set(FScaleMatrix(ViewScale) * EffLocalToWorld, FScaleMatrix(ViewScale) * EffLocalToWorld, GetBounds(), GetLocalBounds(), true, false, AlwaysHasVelocity());

			BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer_HandleUp.Indices.Num() / 3;

			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffers_HandleUp.PositionVertexBuffer.GetNumVertices() - 1;

			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			Mesh.bCanApplyViewModeOverrides = false;

			Collector.AddMesh(ViewIndex, Mesh);
		}
	}
}

FPrimitiveViewRelevance FBangoPlungerSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bDynamicRelevance = true;

	Result.bShadowRelevance = IsShadowCast(View);
	Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
	Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
	
	return Result;
}

void FBangoPlungerSceneProxy::OnTransformChanged()
{
	Origin = GetLocalToWorld().GetOrigin();
}

uint32 FBangoPlungerSceneProxy::GetMemoryFootprint() const
{
	return (sizeof(*this) + GetAllocatedSize());
}

void BuildBoxVerts(const FVector3f& StaticOffset, TArray<FDynamicMeshVertex>& OutVerts, TArray<uint32>& OutIndices)
{
	// Calculate verts for a face pointing down Z
	FVector3f Positions[4] =
	{
		FVector3f(-15, -15, +15),
		FVector3f(-15, +15, +15),
		FVector3f(+15, +15, +15),
		FVector3f(+15, -15, +15)
	};
	FVector2f UVs[4] =
	{
		FVector2f(0,0),
		FVector2f(0,1),
		FVector2f(1,1),
		FVector2f(1,0),
	};

	// Then rotate this face 6 times
	FRotator3f FaceRotations[6];
	FaceRotations[0] = FRotator3f(  0,  0,  0);
	FaceRotations[1] = FRotator3f( 90,  0,  0);
	FaceRotations[2] = FRotator3f(-90,  0,  0);
	FaceRotations[3] = FRotator3f(  0,  0, 90);
	FaceRotations[4] = FRotator3f(  0,  0,-90);
	FaceRotations[5] = FRotator3f(180,  0,	0);

	int Index = 0;
	
	for(int32 f = 0; f < 6; f++)
	{
		FMatrix44f FaceTransform = FRotationMatrix44f(FaceRotations[f]);

		TArray<FDynamicMeshVertex> FaceVerts;
		TArray<uint32> FaceIndices;

		int index = 0;
		
		for(int32 VertexIndex = 0; VertexIndex < 4; VertexIndex++)
		{
			FDynamicMeshVertex V;
			V.Position = StaticOffset + FaceTransform.TransformPosition(Positions[VertexIndex]);
			V.TextureCoordinate[0] = UVs[VertexIndex];
			V.TangentX = FaceTransform.TransformVector(FVector3f(1,0,0));
			V.SetTangents
			(
				FaceTransform.TransformVector(FVector3f(1,0,0)),
				FaceTransform.TransformVector(FVector3f(0,1,0)),
				FaceTransform.TransformVector(FVector3f(0,0,1))
			);
			V.Color = FColor::White;

			FaceVerts.Add(V);

			index++;
		}

		OutVerts.Add(FaceVerts[3]);
		OutVerts.Add(FaceVerts[2]);
		OutVerts.Add(FaceVerts[0]);
		OutVerts.Add(FaceVerts[2]);
		OutVerts.Add(FaceVerts[1]);
		OutVerts.Add(FaceVerts[0]);

		for (int i = 0; i < 6; i++)
		{
			OutIndices.Add(OutIndices.Num());
		}
	}
}

void BuildCylinderVerts(const FVector& Base, const FVector& XAxis, const FVector& YAxis, const FVector& ZAxis, double Radius, double HalfHeight, uint32 Sides, TArray<FDynamicMeshVertex>& OutVerts, TArray<uint32>& OutIndices)
{
	const float	AngleDelta = 2.0f * UE_PI / Sides;
	FVector	LastVertex = Base + XAxis * Radius;

	FVector2D TC = FVector2D(0.0f, 0.0f);
	float TCStep = 1.0f / Sides;

	FVector TopOffset = HalfHeight * ZAxis;

	int32 BaseVertIndex = OutVerts.Num();

	//Compute vertices for base circle.
	for (uint32 SideIndex = 0; SideIndex < Sides; SideIndex++)
	{
		const FVector Vertex = Base + (XAxis * FMath::Cos(AngleDelta * (SideIndex + 1)) + YAxis * FMath::Sin(AngleDelta * (SideIndex + 1))) * Radius;
		FVector Normal = Vertex - Base;
		Normal.Normalize();

		FDynamicMeshVertex MeshVertex;

		MeshVertex.Position = FVector3f(Vertex - TopOffset);
		MeshVertex.TextureCoordinate[0] = FVector2f(TC);

		MeshVertex.SetTangents(
			(FVector3f)-ZAxis,
			FVector3f((-ZAxis) ^ Normal),
			(FVector3f)Normal
			);

		OutVerts.Add(MeshVertex); //Add bottom vertex

		LastVertex = Vertex;
		TC.X += TCStep;
	}

	LastVertex = Base + XAxis * Radius;
	TC = FVector2D(0.0f, 1.0f);

	//Compute vertices for the top circle
	for (uint32 SideIndex = 0; SideIndex < Sides; SideIndex++)
	{
		const FVector Vertex = Base + (XAxis * FMath::Cos(AngleDelta * (SideIndex + 1)) + YAxis * FMath::Sin(AngleDelta * (SideIndex + 1))) * Radius;
		FVector Normal = Vertex - Base;
		Normal.Normalize();

		FDynamicMeshVertex MeshVertex;

		MeshVertex.Position = FVector3f(Vertex + TopOffset);
		MeshVertex.TextureCoordinate[0] = FVector2f(TC);

		MeshVertex.SetTangents(
			(FVector3f)-ZAxis,
			FVector3f((-ZAxis) ^ Normal),
			(FVector3f)Normal
			);

		OutVerts.Add(MeshVertex); //Add top vertex

		LastVertex = Vertex;
		TC.X += TCStep;
	}

	//Add top/bottom triangles, in the style of a fan.
	//Note if we wanted nice rendering of the caps then we need to duplicate the vertices and modify
	//texture/tangent coordinates.
	for (uint32 SideIndex = 1; SideIndex < Sides; SideIndex++)
	{
		int32 V0 = BaseVertIndex;
		int32 V1 = BaseVertIndex + SideIndex;
		int32 V2 = BaseVertIndex + ((SideIndex + 1) % Sides);

		//bottom
		OutIndices.Add(V0);
		OutIndices.Add(V1);
		OutIndices.Add(V2);

		// top
		OutIndices.Add(Sides + V2);
		OutIndices.Add(Sides + V1);
		OutIndices.Add(Sides + V0);
	}

	//Add sides.

	for (uint32 SideIndex = 0; SideIndex < Sides; SideIndex++)
	{
		int32 V0 = BaseVertIndex + SideIndex;
		int32 V1 = BaseVertIndex + ((SideIndex + 1) % Sides);
		int32 V2 = V0 + Sides;
		int32 V3 = V1 + Sides;

		OutIndices.Add(V0);
		OutIndices.Add(V2);
		OutIndices.Add(V1);

		OutIndices.Add(V2);
		OutIndices.Add(V3);
		OutIndices.Add(V1);
	}
}