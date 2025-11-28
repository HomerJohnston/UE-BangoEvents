#include "Bango/Triggers/TargetLocators/Base/BangoTargetLocatorQueryParams.h"

bool FBangoTargetLocatorQueryParams::IsValidMobility(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}
	
	if (QueryType == EBangoTargetQueryMobileType::AllObjects)
	{
		return true;
	}
	
	if (USceneComponent* RootComponent = Actor->GetRootComponent())
	{
		EComponentMobility::Type Mobility = Actor->GetRootComponent()->GetMobility();
		
		switch (QueryType)
		{
			case EBangoTargetQueryMobileType::AllDynamicObjects:
			{
				return Mobility == EComponentMobility::Type::Movable;
			}
			case EBangoTargetQueryMobileType::AllStaticObjects:
			{
				return Mobility == EComponentMobility::Type::Static;
			}
		}	
	}
	
	return false;
}
