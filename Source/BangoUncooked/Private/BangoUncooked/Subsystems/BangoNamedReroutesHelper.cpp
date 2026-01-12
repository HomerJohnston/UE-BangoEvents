#include "BangoNamedReroutesHelper.h"

#include "Editor.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoDestination.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoStart.h"

TMap<FName, TArray<UEdGraphPin*>> UBangoNamedReroutesHelper::DestinationConnections;
TMap<FName, UEdGraphPin*> UBangoNamedReroutesHelper::SourceConnections;
TMap<UEdGraphPin*, UEdGraphPin*> UBangoNamedReroutesHelper::DestinationReroutes;

void UBangoNamedReroutesHelper::Initialize(FSubsystemCollectionBase& Collection)
{
	if (!GEditor)
	{
		return;
	}
	
	GEditor->OnBlueprintPreCompile().AddStatic(&ThisClass::OnBlueprintPreCompile);
}

UEdGraphPin* UBangoNamedReroutesHelper::GetSource(FName Name)
{
	UEdGraphPin** Source = SourceConnections.Find(Name);
	
	if (Source)
	{
		return *Source;
	}
	
	return nullptr;
}

const TArray<UEdGraphPin*> UBangoNamedReroutesHelper::GetDestinations(FName Name)
{
	TArray<UEdGraphPin*>* Destinations = DestinationConnections.Find(Name);
	
	if (!Destinations)
	{
		return {};
	}
	
	return *Destinations;
}

void UBangoNamedReroutesHelper::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	DestinationConnections.Empty();
	DestinationReroutes.Empty();

	for (const TObjectPtr<UEdGraph>& Ubergraph : Blueprint->UbergraphPages)
	{
		for (const TObjectPtr<class UEdGraphNode>& Node : Ubergraph->Nodes)
		{
			if (auto* GotoSource = Cast<UK2Node_BangoGotoStart>(Node))
			{
				//GotoSources.Add(GotoSource);
			}
			if (auto* GotoDestination = Cast<UK2Node_BangoGotoDestination>(Node))
			{
				RegisterDestination(GotoDestination);
			}
		}
	}
}

void UBangoNamedReroutesHelper::RegisterDestination(UK2Node_BangoGotoDestination* Destination)
{
	//GotoDestinations.Add(Destination);
	
	if (Destination->GetRerouteName().IsNone() || !Destination->GetThenPin()->HasAnyConnections())
	{
		return;
	}
	
	TArray<UEdGraphPin*>& Destinations = DestinationConnections.FindOrAdd(Destination->GetRerouteName());

	Destinations.Add(Destination->GetExecPin());
}

void UBangoNamedReroutesHelper::UnregisterDestination(UK2Node_BangoGotoDestination* Destination)
{
	DestinationConnections.Remove(Destination->GetRerouteName());
}

/*
void UBangoNamedReroutesHelper::RegisterDestinationReroute(UEdGraphPin* OldPinAddress, UEdGraphPin* NewPinAddress)
{
	DestinationReroutes.Add(OldPinAddress, NewPinAddress);
}
*/

UEdGraphPin* UBangoNamedReroutesHelper::GetReroutedPin(UEdGraphPin* OriginalPinAddress)
{
	UEdGraphPin** Reroute = DestinationReroutes.Find(OriginalPinAddress);
	
	if (Reroute)
	{
		return *Reroute;
	}
	
	return OriginalPinAddress;
}

