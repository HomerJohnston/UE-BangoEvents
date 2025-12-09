#include "Bango/Core/BangoScriptContainer.h"

#if WITH_EDITOR
void FBangoScriptContainer::PrepareForDestroy()
{
	Guid.Invalidate();
	ScriptClass = nullptr;
}
#endif
