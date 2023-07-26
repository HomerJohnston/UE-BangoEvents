// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Puny/Editor/DebugUtility.h"

void BangoUtility::DebugDraw::DebugDrawDashedLine(UWorld* World, const FVector& Start, const FVector& End, float DashLength, const FColor& Color, bool bPersistentLines, float Lifetime, uint8 DepthPriority, float Thickness)
{
	float Distance = (End - Start).Size();

	FRay Ray(Start, End - Start);
	
	DebugDrawDashedLine(World, Ray, Distance, DashLength, Color, bPersistentLines, Lifetime, DepthPriority, Thickness);
}

void BangoUtility::DebugDraw::DebugDrawDashedLine(UWorld* World, const FVector& Start, const FVector& End, int32 NumDashes, const FColor& Color, bool bPersistentLines, float Lifetime, uint8 DepthPriority, float Thickness)
{
	float Distance = (End - Start).Size();

	float DashLength = Distance / NumDashes;

	FRay Ray(Start, End - Start);
	
	DebugDrawDashedLine(World, Ray, Distance, DashLength, Color, bPersistentLines, Lifetime, DepthPriority, Thickness);
}

void BangoUtility::DebugDraw::DebugDrawDashedLine(UWorld* World, const FRay& Ray, float Distance, float DashLength, const FColor& Color, bool bPersistentLines, float Lifetime, uint8 DepthPriority, float Thickness)
{
	FVector Dir = Ray.Direction;

	if (Dir == FVector::ZeroVector)
	{
		return;
	}
	
	int32 TotalSegments = 1 + Distance / (DashLength + DashLength);
	int32 CurrentSegment = 0;

	while (CurrentSegment < TotalSegments)
	{
		FVector DashStart = Ray.Origin + CurrentSegment * (DashLength + DashLength) * Dir;

		FVector DashEnd = DashStart + DashLength * Dir;

		if ((DashEnd - Ray.Origin).SizeSquared() > FMath::Square(Distance))
		{
			DashEnd = Ray.Origin + Distance * Ray.Direction;
		}

		DrawDebugLine(World, DashStart, DashEnd, Color, bPersistentLines, Lifetime, DepthPriority, Thickness);

		CurrentSegment++;
	}
}
