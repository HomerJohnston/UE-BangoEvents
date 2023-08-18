// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once
#include "Bango/Core/BangoTriggerSignal.h"

#include "BangoInstigatorRecords.generated.h"

enum class EBangoEventSignalType : uint8;

USTRUCT()
struct FBangoInstigatorRecord
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
struct FBangoInstigatorRecordCollection
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<UObject*, FBangoInstigatorRecord> AllInstigatorRecords;

	UPROPERTY()
	TSet<UObject*> ActiveInstigators;

	UPROPERTY()
	UObject* FirstInstigator = nullptr;
	
public:
	TMap<UObject*, FBangoInstigatorRecord> GetData();

	FBangoInstigatorRecord* FindRecordFor(UObject* PotentialInstigator);

	bool IsInstigatorActive(UObject* Instigator);

	int32 GetNumActiveInstigators();
	
	void UpdateInstigatorRecord(UObject* Instigator, EBangoEventSignalType SignalType, double Time);

	bool GetInstigationTime(UObject* Instigator, EBangoEventSignalType SignalType, double& OutTime);

	UObject* GetFirstInstigator();

	void ClearActiveInstigators();
};