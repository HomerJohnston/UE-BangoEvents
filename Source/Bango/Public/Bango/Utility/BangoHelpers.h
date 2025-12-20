#pragma once

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
};
#endif

#if WITH_EDITOR
namespace Bango
{	
	
	bool BANGO_API IsComponentInEditedLevel(UActorComponent* Component);
}
#endif