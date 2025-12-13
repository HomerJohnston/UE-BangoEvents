#include "Bango/Core/BangoScriptContainer.h"

#include "Bango/Core/BangoScript.h"

#if WITH_EDITOR
void FBangoScriptContainer::Unset()
{
	Guid.Invalidate();
	ScriptClass = nullptr;
}

void FBangoScriptContainer::ForceSave()
{
	if (ScriptClass)
	{
		UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(ScriptClass); 
		
		if (Blueprint)
		{
			Blueprint->ForceSave();
		}
	}
	
	
	
}
#endif
