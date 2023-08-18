#include "Bango/Default/InstigatorFilters/BangoInstigatorFilter_ByClasses.h"

bool UBangoInstigatorFilter_ByClasses::IsValidInstigator_Implementation(UObject* Instigator)
{
	bool bAllow = bInvertBehavior;

	TArray<TSubclassOf<UObject>>& PrimaryList = bInvertBehavior ? IgnoreList : AllowList;
	TArray<TSubclassOf<UObject>>& SecondaryList = bInvertBehavior ? AllowList : IgnoreList;

	bool& bIgnoreChildClassesForPrimaryList = bInvertBehavior ? bIgnoreChildClassesForIgnoreList : bIgnoreChildClassesForAllowList;
	bool& bIgnoreChildClassesForSecondaryList = bInvertBehavior ? bIgnoreChildClassesForAllowList : bIgnoreChildClassesForIgnoreList;
	
	for (TSubclassOf<UObject> Class : PrimaryList)
	{
		if (bIgnoreChildClassesForPrimaryList)
		{
			if (Instigator->GetClass() == Class)
			{
				bAllow = !bAllow;
				continue;;
			}
		}
		else
		{
			if (Instigator->IsA(Class))
			{
				bAllow = !bAllow;
				continue;
			}
		}
	}

	if (!bAllow && !bInvertBehavior)
	{
		return false;
	}

	if (bAllow && bInvertBehavior)
	{
		return true;
	}
	
	for (TSubclassOf<UObject> Class : SecondaryList)
	{
		if (bIgnoreChildClassesForSecondaryList)
		{
			if (Instigator->GetClass() == Class)
			{
				bAllow = !bAllow;
				continue;
			}
		}
		else
		{
			if (Instigator->IsA(Class))
			{
				bAllow = !bAllow;
				continue;
			}
		}
	}
	
	return bAllow;
}
