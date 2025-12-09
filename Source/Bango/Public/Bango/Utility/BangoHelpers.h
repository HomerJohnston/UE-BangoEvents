#pragma once

class UBangoScriptBlueprint;
class UBangoScriptComponent;

#if WITH_EDITOR
struct FBangoEditorDelegates
{
	BANGO_API static TMulticastDelegate<void(UBangoScriptComponent*, UBangoScriptBlueprint*)> OnScriptComponentCreated;
	BANGO_API static TMulticastDelegate<void(UBangoScriptComponent*)> OnScriptComponentDestroyed;
};
#endif

namespace Bango
{	
	bool IsComponentInEditedLevel(UActorComponent* Component);
}
