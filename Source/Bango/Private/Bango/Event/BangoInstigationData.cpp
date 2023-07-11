#include "Bango/Event/BangoEventInstigation.h"

FBangoEventInstigation::FBangoEventInstigation() : Instigator(nullptr), Time(INDEX_NONE)
{
}

FBangoEventInstigation::FBangoEventInstigation(UObject* InInstigator, double InTime) : Instigator(InInstigator), Time(InTime)
{
}

bool FBangoEventInstigation::operator==(const FBangoEventInstigation& Other) const
{
	return (Instigator == Other.Instigator);
}

bool FBangoEventInstigation::Equals(UObject* InInstigator) const
{
	return (Instigator == InInstigator);
}
