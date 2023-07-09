#include "Bango/Core/BangoInstigationData.h"

FBangoInstigationData::FBangoInstigationData() : Instigator(nullptr), Time(INDEX_NONE)
{
}

FBangoInstigationData::FBangoInstigationData(UObject* InInstigator, double InTime) : Instigator(InInstigator), Time(InTime)
{
}

bool FBangoInstigationData::operator==(const FBangoInstigationData& Other) const
{
	return (Instigator == Other.Instigator);
}

bool FBangoInstigationData::Equals(UObject* InInstigator) const
{
	return (Instigator == InInstigator);
}
