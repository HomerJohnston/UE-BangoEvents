#pragma once

#include "Containers/UnrealString.h"
#include "Misc/Guid.h"

class UObject;
struct FBangoScriptContainer;
class UBangoScriptBlueprint;
class UBangoScript;
class UBangoScriptComponent;

#if WITH_EDITOR
struct FBangoEditorDelegates
{
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name, bool bImmediate)> OnScriptContainerCreated;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, TSoftClassPtr<UBangoScript> ScriptContainer)> OnScriptContainerDestroyed;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UObject* Outer, FBangoScriptContainer* ScriptContainer, FString Name)> OnScriptContainerDuplicated;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(FGuid ScriptID, UBangoScriptBlueprint*& FoundBlueprint)> OnBangoActorComponentUndoDelete;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(AActor* Actor)> RequestNewID;
	BANGOEDITORTOOLING_API static TMulticastDelegate<void(UBangoScriptComponent* ScriptComponent)> OnScriptComponentClicked;
};
#endif
