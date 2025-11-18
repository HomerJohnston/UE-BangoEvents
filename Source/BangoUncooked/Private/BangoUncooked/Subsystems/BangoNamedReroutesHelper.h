#pragma once

#include "EditorSubsystem.h"

#include "BangoNamedReroutesHelper.generated.h"

class UK2Node_BangoGotoStart;
class UK2Node_BangoGotoDestination;

UCLASS()
class UBangoNamedReroutesHelper : public UEditorSubsystem
{
	GENERATED_BODY()

protected:
	static TMap<FName, UEdGraphPin*> SourceConnections;
	
	static TMap<FName, TArray<UEdGraphPin*>> DestinationConnections;
	
	static TMap<UEdGraphPin*, UEdGraphPin*> DestinationReroutes;
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	static UEdGraphPin* GetSource(FName Name);
	
	static const TArray<UEdGraphPin*> GetDestinations(FName Name);

	static void OnBlueprintPreCompile(UBlueprint* Blueprint);
	
	static void RegisterDestination(UK2Node_BangoGotoDestination* Destination);
	
	static void UnregisterDestination(UK2Node_BangoGotoDestination* Destination);
	
	//static void RegisterDestinationReroute(UEdGraphPin* OldPinAddress, UEdGraphPin* NewPinAddress);
	
	static UEdGraphPin* GetReroutedPin(UEdGraphPin* OriginalPinAddress);
};