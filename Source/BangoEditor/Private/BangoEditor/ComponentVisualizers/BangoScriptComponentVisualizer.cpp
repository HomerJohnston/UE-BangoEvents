#include "BangoScriptComponentVisualizer.h"

#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"
#include "Bango/Utility/BangoEditorUtility.h"

void FBangoScriptComponentVisualizer::OnRegister()
{
	FComponentVisualizer::OnRegister();
}

void FBangoScriptComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{	
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
	
	
	/*
	for (const TWeakObjectPtr<UK2Node_BangoFindActor> WeakNode : UK2Node_BangoFindActor::Nodes)
	{
		if (!WeakNode.IsValid())
		{
			continue;
		}

		UObject* OOO = WeakNode->GetGraph()->GetOuter();
		
		if (OOO->IsSelected())
		{
			UE_LOG(LogTemp, Display, TEXT("Yes"));
		}
	}
	*/
	
	for (const UEdGraph* Graph : Graphs)
	{
		TArray<UK2Node_BangoFindActor*> FindActorNodes;
		Graph->GetNodesOfClass(FindActorNodes);
		
		for (const UK2Node_BangoFindActor* Node : FindActorNodes)
		{
			UObject* OOO = Node->GetGraph()->GetOuter();
			
			TSoftObjectPtr<AActor> TargetActor = Node->GetTargetActor();
			
			if (TargetActor.IsValid())
			{
				FVector TargetOrigin;
				FVector TargetBoxExtents;
				TargetActor->GetActorBounds(false, TargetOrigin, TargetBoxExtents);
				float TargetSphereRadius = TargetBoxExtents.GetMax() * 0.677f;
				
				FVector TargetPos = TargetActor->GetActorLocation();
				
				FVector ViewDir = View->GetViewDirection();
				FVector CircleDir = FVector::UpVector.Cross(ViewDir);
				
				//DrawCircle(PDI, TargetPos, ViewDir.UpVector, CircleDir, FLinearColor::Red, TargetSphereRadius, 16, 0, 10.0f);
				
				FVector Delta = TargetActor->GetActorLocation() - ComponentActor->GetActorLocation();
				
				const float StartDrawDistance = 100.0f;
				
				if (Delta.SizeSquared() > FMath::Square(StartDrawDistance))
				{
					FVector Norm = Delta.GetSafeNormal();
					
					// We lift the connection lines up very slightly for the common case where the actors are both on a flat floor
					FVector Start = ComponentActor->GetActorLocation() + StartDrawDistance * Norm + 10.0f * FVector::UpVector;
					FVector End = TargetActor->GetActorLocation() - TargetSphereRadius * Norm + 10.0f * FVector::UpVector;
					
					//PDI->DrawLine(Start, End, FLinearColor::Red, 0, 1.0);
					//DrawDebugLine(ComponentActor->GetWorld(), Start, End, FColor::Red);
				}
			}
		}
	}
}

struct FBangoActorNodeDraw
{
	const AActor* Actor = nullptr;
	bool bFocused = false;
	
	bool operator==(const FBangoActorNodeDraw& Other) const { return Other.Actor == this->Actor; }
	
	friend uint32 GetTypeHash(const FBangoActorNodeDraw& Struct)
	{
		return GetTypeHash(Struct.Actor);
	}
};



void FBangoScriptComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport,	const FSceneView* View, FCanvas* Canvas)
{
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
	
	FVector ComponentActorScreenPos;
	float ComponentActorSize;
	if (!GetActorScreenPosAndSize(View, Canvas, ComponentActor, ComponentActorScreenPos, ComponentActorSize))
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
					DrawRecord.Actor = Actor;
					
					bool bAlreadyInSet;
					FBangoActorNodeDraw& Draw = VisitedActors.FindOrAddByHash(GetTypeHash(Actor), DrawRecord, &bAlreadyInSet);
					Draw.bFocused = Draw.bFocused || Node->bIsSelected;
				}
			}
			
			// Now we draw
			for (const FBangoActorNodeDraw& DrawInfo : VisitedActors)// int32 i = 0; i < VisitedActors FindActorNodes.Num(); ++i)
			{
				float Saturation = DrawInfo.bFocused ? 1.0f : 0.75f;
				float Luminosity = DrawInfo.bFocused ? 1.0f : 0.75f;
				float Thickness = DrawInfo.bFocused ? 3.0f : 1.0f;
				FLinearColor Color = Bango::Editor::Color::GetHashedColor(GetTypeHash(DrawInfo.Actor), Saturation, Luminosity);
				
				// Draw circle
				FVector ScreenPos;
				float Radius;
				if (!GetActorScreenPosAndSize(View, Canvas, DrawInfo.Actor, ScreenPos, Radius))
				{
					return;
				}
				
				Radius = FMath::Clamp(Radius, 25.0f, 250.0f);
				DrawScreenCircleOverWorldPos(View, Canvas, ScreenPos, Radius, Thickness, Color);					
				
				// Draw connection line
				FVector Delta = DrawInfo.Actor->GetActorLocation() - ComponentActor->GetActorLocation();
				
				const float StartDrawDistance = 100.0f;
				
				if (Delta.SizeSquared() > FMath::Square(StartDrawDistance))
				{
					FVector Start = ComponentActor->GetActorLocation();
					FVector End = DrawInfo.Actor->GetActorLocation();
			
					DrawScreenLineForWorldPos(View, Canvas, Start, End, Thickness, Color, ComponentActorSize * 1.5f, Radius);
				}
			}
		}
		else
		{
			// Fast path - just display count stuff
		}
	}
}

void FBangoScriptComponentVisualizer::DrawScreenCircleOverWorldPos(const FSceneView* View, FCanvas* Canvas, const FVector& WorldPosition, float Radius, float Thickness, const FLinearColor& Color)
{
	uint8 NumLineSegments = 24;
	static TArray<FVector2D> TempPoints;
	TempPoints.Empty(NumLineSegments);

	const float NumFloatLineSegments = (float)NumLineSegments;
	for (uint8 i = 0; i <= NumLineSegments; ++i)
	{
		const float Angle = (i / NumFloatLineSegments) * TWO_PI;

		FVector2D PointOnCircle;
		PointOnCircle.X = cosf(Angle) * Radius + WorldPosition.X;
		PointOnCircle.Y = sinf(Angle) * Radius + WorldPosition.Y;
		TempPoints.Add(PointOnCircle);
	}
	
	for (uint8 i = 0; i <= NumLineSegments; ++i)
	{
		uint8 Index0 = i;
		uint8 Index1 = (i + 1) % NumLineSegments;
		
		FCanvasLineItem Line(TempPoints[Index0], TempPoints[Index1]);
		Line.LineThickness = Thickness;
		Line.SetColor(Color);
		Canvas->DrawItem(Line);
	}
}

void FBangoScriptComponentVisualizer::DrawScreenLineForWorldPos(const FSceneView* View, FCanvas* Canvas, const FVector& WorldStart, const FVector& WorldEnd, float Thickness, const FLinearColor& Color, float StartCutoff, float EndCutoff)
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

bool FBangoScriptComponentVisualizer::GetActorScreenPosAndSize(const FSceneView* View, FCanvas* Canvas, const AActor* Actor, FVector& ScreenLocation, float& Radius)
{
	FVector TargetOrigin;
	FVector TargetBoxExtents;
	Actor->GetActorBounds(false, TargetOrigin, TargetBoxExtents);
	float TargetSphereRadius = TargetBoxExtents.GetMax() * 0.677f;
	FVector WorldPos = Actor->GetActorLocation();
	
	FVector2D ScreenPos; 
	if (!GetScreenPos(View, WorldPos, ScreenPos))
	{
		return false;
	}

	FVector RightView = FVector::UpVector.Cross(View->GetViewDirection());
	FVector UpView = RightView.Cross(View->GetViewDirection());
	
	FVector2D RadiusScreenPos;
	if (!GetScreenPos(View, WorldPos + TargetSphereRadius * UpView, RadiusScreenPos))
	{
		return false;
	}
	
	ScreenLocation.X = ScreenPos.X;
	ScreenLocation.Y = ScreenPos.Y;

	Radius = FVector2D::Distance(RadiusScreenPos, ScreenPos);
	
	return true;
}

bool FBangoScriptComponentVisualizer::GetScreenPos(const FSceneView* View, const FVector& WorldPos, FVector2D& ScreenPos)
{
	FVector4 ScreenPoint = View->WorldToScreen(WorldPos);

	return View->ScreenToPixel(ScreenPoint, ScreenPos);
}

