#include "BangoScriptReferenceDrawerSubsystem.h"

#include "Selection.h"
#include "Bango/Components/BangoScriptComponent.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"

void UBangoScriptReferenceDrawerSubsystem::Tick(float DeltaTime)
{
	if (!GEditor)
	{
		return;
	}
	
	TArray<AActor*> SelectedActors;
	GEditor->GetSelectedActors()->GetSelectedObjects(SelectedActors);
	
	if (SelectedActors.Num() != 1)
	{
		return;
	}

	AActor* SelectedActor = SelectedActors[0];	
	
	FSoftClassPath Target = UBangoScript::SelectedScript;
	
	if (Target.IsNull())
	{
		return;
	}
	
	TSoftClassPtr<UBangoScript> TargetScriptClass = TSoftClassPtr<UBangoScript>(Target);

	TSubclassOf<UBangoScript> TargetScript = TargetScriptClass.LoadSynchronous();
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(TargetScriptClass);

	if (Blueprint)
	{
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
}

TStatId UBangoScriptReferenceDrawerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FBangoScriptReferenceDrawerSubsystem, STATGROUP_Tickables);	
}

bool UBangoScriptReferenceDrawerSubsystem::IsTickable() const
{
	return true;
}
