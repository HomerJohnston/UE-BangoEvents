

#include "BangoEditorTooling/BangoDebugDrawCanvas.h"

#include "Engine/Canvas.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"

// ----------------------------------------------

/*
float FBangoDebugDrawCanvas::GetNextYOffset(AActor* Actor)
{
	uint32& Index = DrawIndices.FindOrAdd(Actor);
	return Index++ * GetYPadding();
}
*/

// ----------------------------------------------

// ----------------------------------------------

FBangoDebugDrawCanvas::FBangoDebugDrawCanvas(UCanvas* InCanvas): Canvas(InCanvas)
{
	
}

FVector FBangoDebugDrawCanvas::GetNextScreenPos(AActor* Actor)
{
	FCanvasDrawLocation& DrawLocation = DrawLocations.FindOrAdd(Actor);
	
	if (DrawLocation.LineIndex == 0)
	{
		FVector ScreenLocation;
		if (GetScreenLocationAboveActor(Actor, ScreenLocation))
		{
			DrawLocation.InitialPositon = ScreenLocation;
		}
	}

	return DrawLocation.InitialPositon + FVector(0.0f, GetLineHeight() * DrawLocation.LineIndex++, 0.0f);
}

// ----------------------------------------------

float FBangoDebugDrawCanvas::GetAlpha(AActor* Actor) const
{
	check(Canvas);
	check(Actor);
	
	FVector CameraPos;
	GetCameraPos(CameraPos);
	
	float MinDistanceSq = FMath::Square(MinDrawDistance);
	float MaxDistanceSq = FMath::Square(MaxDrawDistance);
	float DistanceSq = FVector::DistSquared(CameraPos, Actor->GetActorLocation());
	
	// This isn't linear but I'm OK with that for debug drawing
	float LerpAlpha = FMath::Clamp((DistanceSq - MinDistanceSq) / (MaxDistanceSq - MinDistanceSq), 0.0f, 1.0f);
	
	return 1.0f - LerpAlpha;
}

// ----------------------------------------------

float FBangoDebugDrawCanvas::GetLineHeight() const
{
	return 32.0f;
}

// ----------------------------------------------

void FBangoDebugDrawCanvas::GetCameraPos(FVector& CameraPos) const
{
	check(Canvas);

	FVector DummyDir;
	GetCameraPos(CameraPos, DummyDir);
}

// ----------------------------------------------

void FBangoDebugDrawCanvas::GetCameraPos(FVector& CameraPos, FVector& CameraDir) const
{
	check(Canvas);
	
	double X, Y;
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), CameraPos, CameraDir);
}

bool FBangoDebugDrawCanvas::GetScreenLocationAboveActor(AActor* Actor, FVector& ScreenLocation) const
{
	check(Canvas);
	check(Actor);
	
	FVector TargetOrigin;
	FVector TargetBoxExtents;
	Actor->GetActorBounds(false, TargetOrigin, TargetBoxExtents);
	float ActorHeight = TargetBoxExtents.Z * 0.5f + HeightAboveActor;
	
	ScreenLocation = Canvas->Project(Actor->GetActorLocation() + FVector(0.0f, 0.0f, ActorHeight), false);
	
	if (ScreenLocation.Z < 0.0f)
	{
		return false;
	}
	
	return true;
}
