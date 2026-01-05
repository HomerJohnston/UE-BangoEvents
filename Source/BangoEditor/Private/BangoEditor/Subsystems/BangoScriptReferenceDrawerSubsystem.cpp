#include "BangoScriptReferenceDrawerSubsystem.h"

#include "Selection.h"
#include "Bango/Components/BangoScriptComponent.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"

void UBangoScriptReferenceDrawerSubsystem::Tick(float DeltaTime)
{
	AActor* SelectedActor;
	UBangoScriptBlueprint* Blueprint;
	
	if (!GetBlueprintAndActor(Blueprint, SelectedActor))
	{
		return;
	}
	
	TArray<UEdGraph*> Graphs;
	Blueprint->GetAllGraphs(Graphs);
	
	for (const UEdGraph* Graph : Graphs)
	{
		TArray<UK2Node_BangoFindActor*> FindActorNodes;
		Graph->GetNodesOfClass(FindActorNodes);
		
		for (const UK2Node_BangoFindActor* Node : FindActorNodes)
		{
			TSoftObjectPtr<AActor> TargetActor = Node->GetTargetActor();
			
			if (TargetActor.IsValid())
			{
				FVector TargetOrigin;
				FVector TargetBoxExtents;
				TargetActor->GetActorBounds(false, TargetOrigin, TargetBoxExtents);
				float TargetSphereRadius = FMath::Clamp(TargetBoxExtents.GetMax() * 0.5f, 50.0f, 200.0f);
				DrawDebugSphere(TargetActor->GetWorld(), TargetActor->GetActorLocation(), TargetSphereRadius, 16, FColor::Red);
				
				FVector Delta = TargetActor->GetActorLocation() - SelectedActor->GetActorLocation();
				
				const float StartDrawDistance = 100.0f;
				
				if (Delta.SizeSquared() > FMath::Square(StartDrawDistance))
				{
					FVector Norm = Delta.GetSafeNormal();
					
					// We lift the connection lines up very slightly for the common case where the actors are both on a flat floor
					FVector Start = SelectedActor->GetActorLocation() + StartDrawDistance * Norm + 10.0f * FVector::UpVector;
					FVector End = TargetActor->GetActorLocation() - TargetSphereRadius * Norm + 10.0f * FVector::UpVector;
					
					DrawDebugLine(SelectedActor->GetWorld(), Start, End, FColor::Red);
				}
			}
		}
	}
}

TStatId UBangoScriptReferenceDrawerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FBangoScriptReferenceDrawerSubsystem, STATGROUP_Tickables);	
}

bool UBangoScriptReferenceDrawerSubsystem::IsTickable() const
{
	return true;
}

bool UBangoScriptReferenceDrawerSubsystem::GetBlueprintAndActor(UBangoScriptBlueprint*& Blueprint, AActor*& SelectedActor)
{
	if (!GEditor)
	{
		return false;
	}
	
	TArray<AActor*> SelectedActors;
	GEditor->GetSelectedActors()->GetSelectedObjects(SelectedActors);
	
	if (SelectedActors.Num() != 1)
	{
		return false;
	}

	SelectedActor = SelectedActors[0];	
	if (!IsValid(SelectedActor))
	{
		return false;
	}
	
	FSoftClassPath Target = UBangoScript::SelectedScript;
	
	if (Target.IsNull())
	{
		return false;
	}
	
	TSoftClassPtr<UBangoScript> TargetScriptClass = TSoftClassPtr<UBangoScript>(Target);
	if (TargetScriptClass.IsNull())
	{
		return false;
	}

	// This throws a warning when the script is deleted. This will be fixed once I build a proper component visualizer instead of this stupid subsystem.
	TSubclassOf<UBangoScript> TargetScript = TargetScriptClass.LoadSynchronous();
	if (!TargetScript)
	{
		return false;
	}
	
	Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(TargetScriptClass);
	if (!Blueprint)
	{
		return false;
	}
	
	return true;
}
