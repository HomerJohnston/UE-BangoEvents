// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoScripts/EditorTooling/BangoEditorLog.h"
#include "Containers/UnrealString.h"

class UActorComponent;

namespace Bango
{
	namespace Debug
	{
		BANGOSCRIPTS_EDITORTOOLING_API void PrintComponentState(UActorComponent* Component, FString Msg);

		BANGOSCRIPTS_EDITORTOOLING_API void PrintFlagNames();
		
		namespace Draw
		{		
			BANGOSCRIPTS_EDITORTOOLING_API void DebugDrawDashedLine(UWorld* World, const FVector& Start, const FVector& End, float DashLength, const FColor& Color, bool bPersistentLines = false, float Lifetime = 0, uint8 DepthPriority = 0, float Thickness = 0.0f);
		
			BANGOSCRIPTS_EDITORTOOLING_API void DebugDrawDashedLine(UWorld* World, const FVector& Start, const FVector& End, int32 NumDashes, const FColor& Color, bool bPersistentLines = false, float Lifetime = 0, uint8 DepthPriority = 0, float Thickness = 0.0f);

			BANGOSCRIPTS_EDITORTOOLING_API void DebugDrawDashedLine(UWorld* World, const FRay& Ray, float Distance, float DashLength, const FColor& Color, bool bPersistentLines = false, float Lifetime = 0, uint8 DepthPriority = 0, float Thickness = 0.0f);
		}
	}
}
