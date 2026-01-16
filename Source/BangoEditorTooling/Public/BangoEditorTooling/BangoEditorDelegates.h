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

#if WITH_EDITOR
struct FBangoEditorDelegates
{
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name)> OnScriptContainerCreated;
	
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name)> OnScriptContainerDuplicated;
	
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer)> OnScriptContainerDestroyed;
	
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptContainer)> OnScriptContainerUnregisteredDuringTransaction;
	
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(FGuid ScriptID, UBangoScriptBlueprint*& FoundBlueprint)> OnBangoActorComponentUndoDelete;
	
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(AActor* Actor)> RequestNewID;
	
	// 
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UBangoScriptComponent* ScriptComponent)> OnScriptComponentClicked;
	
	//  
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(FBangoDebugDrawCanvas& DebugDrawData, bool bPIE)> BangoDebugDraw;
	
	// FBangoScriptContainerCustomization instances subscribe to this to regenerate themselves when the script subsystem does things to level scripts
	BANGOEDITORTOOLING_API static TMulticastDelegate<void()> OnScriptGenerated;
};
#endif
