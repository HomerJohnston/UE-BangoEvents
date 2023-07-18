#pragma once
#include "PunyEventSignalType.h"

#include "PunyInstigatorRecords.generated.h"

enum class EPunyEventSignalType : uint8;

USTRUCT()
struct FPunyInstigatorRecord
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* Instigator = nullptr;
	
	UPROPERTY()
	double StartTime = -1.0;
	
	UPROPERTY()
	double StopTime = -1.0;
};

USTRUCT()
struct FPunyInstigatorRecordCollection
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<UObject*, FPunyInstigatorRecord> AllInstigatorRecords;

	UPROPERTY()
	TSet<UObject*> ActiveInstigators;

	UPROPERTY()
	UObject* FirstInstigator = nullptr;
	
public:
	TMap<UObject*, FPunyInstigatorRecord> GetData();

	FPunyInstigatorRecord* FindRecordFor(UObject* PotentialInstigator);

	bool IsInstigatorActive(UObject* Instigator);

	int32 GetNumActiveInstigators();
	
	void UpdateInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType, double Time);

	bool GetInstigationTime(UObject* Instigator, EPunyEventSignalType SignalType, double& OutTime);

	UObject* GetFirstInstigator();

	void ClearActiveInstigators();
};