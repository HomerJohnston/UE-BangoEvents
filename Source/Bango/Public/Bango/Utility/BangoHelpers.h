#pragma once

class UBangoActorIDComponent;
struct FBangoScriptContainer;
class UBangoScriptBlueprint;
class UBangoScriptComponent;

#if WITH_EDITOR
struct FBangoEditorDelegates
{
	BANGO_API static TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> OnScriptContainerCreated;
	BANGO_API static TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> OnScriptContainerDestroyed;
	BANGO_API static TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> OnScriptContainerDuplicated;
	
	BANGO_API static TMulticastDelegate<void(FGuid /* Script ID */, UBangoScriptBlueprint*& /* Found Blueprint */)> OnBangoActorComponentUndoDelete;
	
	BANGO_API static TMulticastDelegate<void(AActor* Actor)> RequestNewID;
};
#endif

#if WITH_EDITOR
namespace Bango
{	
	BANGO_API bool IsComponentInEditedLevel(UActorComponent* Component);
	
	BANGO_API UBangoActorIDComponent* GetActorIDComponent(AActor* Actor, bool bForceCreate = false);
	
	BANGO_API FName GetBangoName(AActor* Actor);
}
#endif