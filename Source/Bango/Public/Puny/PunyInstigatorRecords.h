#pragma once

#include "PunyInstigatorRecords.generated.h"

enum class EPunyTriggerSignalType : uint8;
USTRUCT()
struct FPunyInstigatorRecord
{
	GENERATED_BODY()

	FPunyInstigatorRecord();
	
	FPunyInstigatorRecord(EPunyTriggerSignalType InSignalType, double InTime);
	
	UPROPERTY()
	EPunyTriggerSignalType SignalType;
	
	UPROPERTY()
	double Time;
};

USTRUCT()
struct FPunyInstigatorRecords
{
	GENERATED_BODY()

	FPunyInstigatorRecords();

	FPunyInstigatorRecords(UObject* InInstigator);
	
	UPROPERTY()
	UObject* Instigator;

	UPROPERTY()
	TMap<EPunyTriggerSignalType, FPunyInstigatorRecord> SignalRecords;
};