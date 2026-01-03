#pragma once

class UBangoActorIDComponent;
struct FBangoScriptContainer;
class UBangoScriptBlueprint;
class UBangoScriptComponent;

#if WITH_EDITOR
struct FBangoEditorDelegates
{
	BANGO_API static TMulticastDelegate<void(UObject* /*Outer*/, FBangoScriptContainer* /*Script Container*/, FString /*Name*/, bool /*bImmediate*/)> OnScriptContainerCreated;
	BANGO_API static TMulticastDelegate<void(UObject* /*Outer*/, FBangoScriptContainer* /*Script Container*/)> OnScriptContainerDestroyed;
	BANGO_API static TMulticastDelegate<void(UObject* /*Outer*/, FBangoScriptContainer* /*Script Container*/, FString /*Name*/)> OnScriptContainerDuplicated;
	
	BANGO_API static TMulticastDelegate<void(FGuid /*ScriptID*/, UBangoScriptBlueprint*& /*FoundBlueprint*/)> OnBangoActorComponentUndoDelete;
	
	BANGO_API static TMulticastDelegate<void(AActor* Actor)> RequestNewID;
};
#endif

#if WITH_EDITOR
namespace Bango
{	
	BANGO_API bool IsComponentInEditedLevel(UActorComponent* Component);
	
	BANGO_API bool IsBeingEditorDeleted(UActorComponent* Component);
	
	BANGO_API UBangoActorIDComponent* GetActorIDComponent(AActor* Actor, bool bForceCreate = false);
	
	BANGO_API FName GetBangoName(AActor* Actor);
}
#endif