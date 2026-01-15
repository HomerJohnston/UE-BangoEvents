#pragma once

#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "Misc/Guid.h"
#include "UObject/SoftObjectPtr.h"

class UObject;
struct FBangoScriptContainer;
class UBangoScriptBlueprint;
class UBangoScript;
class UBangoScriptComponent;

struct FBangoDebugDrawCanvas;
/*
{
	FBangoDebugDrawCanvas(UCanvas* InCanvas) : Canvas(InCanvas) {};
	
	// Increments DrawIndices and returns DrawIndices[Actor] * GetYPadding
	FVector GetDrawPosition(AActor* Actor);
	
	UCanvas* operator->() { return Canvas; } 

private:
	UCanvas* Canvas = nullptr;

	float GetYPadding() const; 
	
	TMap<AActor*, uint32> DrawIndices;
};
*/

#if WITH_EDITOR
struct FBangoEditorDelegates
{
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name)> OnScriptContainerCreated;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name)> OnScriptContainerDuplicated;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptContainer)> OnScriptContainerDestroyed;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptContainer)> OnScriptContainerUnregisteredDuringTransaction;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(FGuid ScriptID, UBangoScriptBlueprint*& FoundBlueprint)> OnBangoActorComponentUndoDelete;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(AActor* Actor)> RequestNewID;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UBangoScriptComponent* ScriptComponent)> OnScriptComponentClicked;
	
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(FBangoDebugDrawCanvas& DebugDrawData, bool bPIE)> BangoDebugDraw;
};
#endif
