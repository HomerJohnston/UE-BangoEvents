#include "Bango/BangoScriptHolder.h"

void FBangoScriptContainer::PrepareForDestroy()
{
	Guid.Invalidate();
	ScriptBlueprint = nullptr;
	ScriptClass = nullptr;
}
