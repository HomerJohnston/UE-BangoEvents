﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

namespace BangoUtility
{
	namespace DebugDraw
	{
		void DebugDrawDashedLine(UWorld* World, const FVector& Start, const FVector& End, float DashLength, const FColor& Color, bool bPersistentLines = false, float Lifetime = 0, uint8 DepthPriority = 0, float Thickness = 0.0f);
		
		void DebugDrawDashedLine(UWorld* World, const FVector& Start, const FVector& End, int32 NumDashes, const FColor& Color, bool bPersistentLines = false, float Lifetime = 0, uint8 DepthPriority = 0, float Thickness = 0.0f);

		void DebugDrawDashedLine(UWorld* World, const FRay& Ray, float Distance, float DashLength, const FColor& Color, bool bPersistentLines = false, float Lifetime = 0, uint8 DepthPriority = 0, float Thickness = 0.0f);
	}
}