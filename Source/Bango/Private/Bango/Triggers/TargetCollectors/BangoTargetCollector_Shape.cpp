#include "Bango/Triggers/TargetCollectors/BangoTargetCollector_Shape.h"

#define LOCTEXT_NAMESPACE "Bango"

UBangoTargetCollector_Shape::UBangoTargetCollector_Shape()
{
	DisplayName = LOCTEXT("BangoTargetCollector_Shape_Name", "Shape");
	
	SortOrder = 10;
}

void UBangoTargetCollector_Shape::Setup()
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

void UBangoTargetCollector_Shape::UpdateCandidateActors(FBangoTargetCollectorQueryParams Params)
{
	
}

#undef LOCTEXT_NAMESPACE