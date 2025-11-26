#pragma once
#include "CookPackageSplitter.h"

class FBangoPackageSplitter : public FGCObject, public ICookPackageSplitter
{
	static bool ShouldSplit(UObject* SplitData);
};
