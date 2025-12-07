#include "Bango/BangoScriptHolder.h"

#if WITH_EDITOR
void FBangoScriptContainer::PrepareForDestroy()
{
	Guid.Invalidate();
	ScriptBlueprint = nullptr;
	ScriptClass = nullptr;
}
#endif
