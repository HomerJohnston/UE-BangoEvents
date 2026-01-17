#include "BangoScripts/Triggers/TargetCollectors/Base/BangoTargetCollectorQueryParams.h"

bool FBangoTargetCollectorQueryParams::IsValidMobility(AActor* Actor)
{
	/*
	if (!IsValid(Actor))
	{
		return false;
	}
	
	if (MobilityTypes == EBangoTargetQueryMobileType::AllObjects)
	{
		return true;
	}
	
	if (USceneComponent* RootComponent = Actor->GetRootComponent())
	{
		EComponentMobility::Type Mobility = Actor->GetRootComponent()->GetMobility();
		
		switch (MobilityTypes)
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
	*/
	return false;
}
