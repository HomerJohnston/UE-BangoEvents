#include "Bango/Triggers/TargetLocators/BangoTargetLocator_Shape.h"

void UBangoTargetLocator_Shape::Setup()
{
	// Build a "tall" box shape for overlap events and attach it to the trigger actor. It needs to fully encompass all of the points.
	
	if (Points.Num() < 3)
	{
		return;
	}
	
	for (FVector Point : Points)
	{
		
	}
}

void UBangoTargetLocator_Shape::UpdateCandidateActors(FBangoTargetLocatorQueryParams Params)
{
	
}
