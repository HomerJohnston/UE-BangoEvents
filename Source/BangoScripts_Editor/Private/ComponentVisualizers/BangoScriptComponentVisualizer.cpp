#include "BangoScriptComponentVisualizer.h"

#include "SceneView.h"
#include "BangoScripts/Components/BangoScriptComponent.h"
#include "BangoScripts/Core/BangoScriptBlueprint.h"
#include "BangoScripts/EditorTooling/BangoColors.h"
#include "BangoScripts/Uncooked/K2Nodes/K2Node_BangoFindActor.h"
#include "BangoScripts/EditorTooling/BangoEditorUtility.h"
#include "Components/Viewport.h"
#include "Framework/Application/SlateApplication.h"

void FBangoScriptComponentVisualizer::OnRegister()
{
	FComponentVisualizer::OnRegister();
}

void FBangoScriptComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{	
}

struct FBangoActorNodeDraw
{
	TSoftObjectPtr<const AActor> Actor = nullptr;
	bool bFocused = false;
	
	bool operator==(const FBangoActorNodeDraw& Other) const { return Other.Actor == this->Actor; }
	
	friend uint32 GetTypeHash(const FBangoActorNodeDraw& Struct)
	{
		return GetTypeHash(Struct.Actor);
	}
};



void FBangoScriptComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport,	const FSceneView* View, FCanvas* Canvas)
{
	if (!IsValid(Component))
	{
		// This can happen if you delete or undo creation of the component or actor
		return;
	}
	
	const AActor* ComponentActor = Component->GetOwner();
	const UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(Component);
	const UBangoScriptBlueprint* Blueprint;
	
	if (!ComponentActor || !ScriptComponent)
	{
		return;
	}
	
	Blueprint = ScriptComponent->GetScriptBlueprint();
	
	if (!Blueprint)
	{
		return;
	}
		
	TArray<UEdGraph*> Graphs;
	Blueprint->GetAllGraphs(Graphs);
	
	FVector ComponentActorWorldPos;
	FVector ComponentActorScreenPos;
	float ComponentActorSize;
	if (!GetActorScreenPosAndSize(View, Canvas, ComponentActor, ComponentActorWorldPos, ComponentActorScreenPos, ComponentActorSize))
	{
		return;
	}
				
	for (const UEdGraph* Graph : Graphs)
	{
		TArray<UK2Node_BangoFindActor*> FindActorNodes;
		Graph->GetNodesOfClass(FindActorNodes);

		TSet<FBangoActorNodeDraw> VisitedActors;

		// TODO this should be a cvar/ini setting
		const int32 MaxActorVisualizationCount = 100;
		
		// This is kind of arbitrary but if some nutjob builds a script with 1000000 actor nodes I don't want to choke the system to a crawl.
		if (FindActorNodes.Num() <= MaxActorVisualizationCount)
		{
			VisitedActors.Reserve(FindActorNodes.Num());
			
			// First we iterate over the whole list to find all actors to draw
			for (const UK2Node_BangoFindActor* Node : FindActorNodes)
			{
				const TSoftObjectPtr<AActor> TargetActor = Node->GetTargetActor();
			
				if (const AActor* Actor = TargetActor.Get())
				{
					FBangoActorNodeDraw DrawRecord;
					DrawRecord.Actor = TargetActor;
					
					bool bAlreadyInSet;
					FBangoActorNodeDraw& Draw = VisitedActors.FindOrAddByHash(GetTypeHash(Actor), DrawRecord, &bAlreadyInSet);
					Draw.bFocused = Draw.bFocused || (GFrameCounter - Node->LastSelectedFrame < 3);
				}
			}
			
			float MinRadius = 3.0f;
			float MaxRadius = 10.0f;

			//MinRadius = FMath::Max(MinRadius, 0.005f * Viewport->GetSizeXY().Y);
			//MaxRadius = FMath::Max(MaxRadius, 0.02f * Viewport->GetSizeXY().Y);
			
			// Now we draw
			for (const FBangoActorNodeDraw& DrawInfo : VisitedActors)// int32 i = 0; i < VisitedActors FindActorNodes.Num(); ++i)
			{
				float Saturation = DrawInfo.bFocused ? 1.0f : 0.8f;
				float Luminosity = DrawInfo.bFocused ? 1.0f : 0.8f;
				float Thickness = DrawInfo.bFocused ? 3.0f : 1.5f;
				FLinearColor Color = Bango::Colors::Funcs::GetHashedColor(GetTypeHash(DrawInfo.Actor), Saturation, Luminosity);
				
				// Draw circle
				FVector TargetActorWorldPos;
				FVector TargetActorScreenPos;
				float Radius;
				if (!GetActorScreenPosAndSize(View, Canvas, DrawInfo.Actor.Get(), TargetActorWorldPos, TargetActorScreenPos, Radius))
				{
					return;
				}
				
				Radius = FMath::Lerp(0.1f * Radius, 0.01f * Viewport->GetSizeXY().Y, 0.5f); // Grow the radius a bit depending on the size of the actor
				Radius = FMath::Clamp(Radius, MinRadius, MaxRadius); // But clamp its max size also
				
				DrawCircle_ScreenSpace(View, Canvas, TargetActorScreenPos, Radius, Thickness, Color);					
				
				// Draw connection line
				FVector Delta = DrawInfo.Actor->GetActorLocation() - ComponentActor->GetActorLocation();
				
				const float StartDrawDistance = 100.0f;
				
				if (Delta.SizeSquared() > FMath::Square(StartDrawDistance))
				{
					FVector Start = ComponentActor->GetActorLocation();
					FVector End = TargetActorWorldPos;
			
					DrawLine_WorldSpace(View, Canvas, Start, End, Thickness, Color, ComponentActorSize * 0.5f, Radius);
				}
			}
		}
		else
		{
			// TODO fast path - just display count stuff
		}
	}
}

void FBangoScriptComponentVisualizer::DrawCircle_ScreenSpace(const FSceneView* View, FCanvas* Canvas, const FVector& ScreenPosition, float Radius, float Thickness, const FLinearColor& Color)
{
	uint8 NumLineSegments = 24;
	static TArray<FVector2D> TempPoints;
	TempPoints.Empty(NumLineSegments);

	const float NumFloatLineSegments = (float)NumLineSegments;
	for (uint8 i = 0; i <= NumLineSegments; ++i)
	{
		const float Angle = (i / NumFloatLineSegments) * TWO_PI;

		FVector2D PointOnCircle;
		PointOnCircle.X = cosf(Angle) * Radius + ScreenPosition.X;
		PointOnCircle.Y = sinf(Angle) * Radius + ScreenPosition.Y;
		TempPoints.Add(PointOnCircle);
	}
	
	for (uint8 i = 0; i <= NumLineSegments; ++i)
	{
		uint8 Index0 = i;
		uint8 Index1 = (i + 1) % NumLineSegments;
		
		FCanvasLineItem Line(TempPoints[Index0], TempPoints[Index1]);
		Line.LineThickness = Thickness;
		Line.SetColor(Color);
		//Canvas->DrawItem(Line);
		
		Canvas->DrawNGon(FVector2D(ScreenPosition.X, ScreenPosition.Y), Color.ToFColor(false), 16, Radius);
	}
}

void FBangoScriptComponentVisualizer::DrawLine_WorldSpace(const FSceneView* View, FCanvas* Canvas, const FVector& WorldStart, const FVector& WorldEnd, float Thickness, const FLinearColor& Color, float StartCutoff, float EndCutoff)
{
	FVector2D ScreenStart;
	FVector2D ScreenEnd;
	
	if (!GetScreenPos(View, WorldStart, ScreenStart))
	{
		return;
	}
	
	if (!GetScreenPos(View, WorldEnd, ScreenEnd))
	{
		return;
	}
	
	if (FVector2D::DistSquared(ScreenStart, ScreenEnd) <= 1.0f)
	{
		return;
	}
	
	if (StartCutoff >= KINDA_SMALL_NUMBER || EndCutoff >= KINDA_SMALL_NUMBER)
	{
		FVector2D Dir = (ScreenEnd - ScreenStart).GetSafeNormal();
		
		ScreenStart = ScreenStart + StartCutoff * Dir;
		ScreenEnd = ScreenEnd - EndCutoff * Dir;
	}
	
	FCanvasLineItem Line(ScreenStart, ScreenEnd);
	Line.LineThickness = Thickness;
	Line.SetColor(Color);
	Canvas->DrawItem(Line);
}

bool FBangoScriptComponentVisualizer::GetActorScreenPosAndSize(const FSceneView* View, FCanvas* Canvas, const AActor* Actor, FVector& OutWorldOrigin, FVector& OutOriginScreenLocation, float& Radius)
{
	FVector TargetBoxExtents;
	Actor->GetActorBounds(false, OutWorldOrigin, TargetBoxExtents);
	float TargetSphereRadius = TargetBoxExtents.GetMax() * 0.677f;
	
	FVector2D ScreenPos; 
	if (!GetScreenPos(View, OutWorldOrigin, ScreenPos))
	{
		return false;
	}

	FVector RightView = FVector::UpVector.Cross(View->GetViewDirection());
	FVector UpView = RightView.Cross(View->GetViewDirection());
	
	FVector2D RadiusScreenPos;
	if (!GetScreenPos(View, OutWorldOrigin + TargetSphereRadius * UpView, RadiusScreenPos))
	{
		return false;
	}
	
	OutOriginScreenLocation.X = ScreenPos.X;
	OutOriginScreenLocation.Y = ScreenPos.Y;

	Radius = FVector2D::Distance(RadiusScreenPos, ScreenPos);
	
	return true;
}

bool FBangoScriptComponentVisualizer::GetScreenPos(const FSceneView* View, const FVector& WorldPos, FVector2D& ScreenPos)
{
	FVector4 ScreenPoint = View->WorldToScreen(WorldPos);

	return View->ScreenToPixel(ScreenPoint, ScreenPos);
}

