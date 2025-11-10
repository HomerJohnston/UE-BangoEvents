#include "Bango/Core/BangoScriptHandle.h"

FBangoScriptHandle::FBangoScriptHandle()
{
	Guid = FGuid::NewGuid();
}

bool FBangoScriptHandle::IsValid()
{
	return Guid.IsValid();
}

void FBangoScriptHandle::Invalidate()
{
	Guid.Invalidate();
}
