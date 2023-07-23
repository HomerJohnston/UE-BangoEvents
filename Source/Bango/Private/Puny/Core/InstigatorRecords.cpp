#include "Puny/Core/InstigatorRecords.h"

#include "Puny/Core/EventSignal.h"

TMap<UObject*, FPunyInstigatorRecord> FPunyInstigatorRecordCollection::GetData()
{ return AllInstigatorRecords; }

FPunyInstigatorRecord* FPunyInstigatorRecordCollection::FindRecordFor(UObject* PotentialInstigator)
{
	return (AllInstigatorRecords.Find(PotentialInstigator));
}

bool FPunyInstigatorRecordCollection::IsInstigatorActive(UObject* Instigator)
{
	if (!IsValid(Instigator))
	{
		return false;
	}
	
	return ActiveInstigators.Contains(Instigator);
}

int32 FPunyInstigatorRecordCollection::GetNumActiveInstigators()
{
	return ActiveInstigators.Num();
}

void FPunyInstigatorRecordCollection::UpdateInstigatorRecord(UObject* Instigator, EPunyEventSignalType SignalType, double Time)
{
	auto& Record = AllInstigatorRecords.FindOrAdd(Instigator);
	Record.Instigator = Instigator;

	switch (SignalType)
	{
		case EPunyEventSignalType::StartAction:
		{
			Record.StartTime = Time;
			
			if (ActiveInstigators.Num() == 0)
			{
				FirstInstigator = Instigator;
			}

			ActiveInstigators.Add(Instigator);
			
			break;
		}
		case EPunyEventSignalType::StopAction:
		{
			Record.StopTime = Time;
			
			ActiveInstigators.Remove(Instigator);

			if (FirstInstigator == Instigator)
			{
				FirstInstigator = nullptr;
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

bool FPunyInstigatorRecordCollection::GetInstigationTime(UObject* Instigator, EPunyEventSignalType SignalType, double& OutTime)
{
	if (FPunyInstigatorRecord* Record = AllInstigatorRecords.Find(Instigator))
	{
		switch (SignalType)
		{
			case EPunyEventSignalType::StartAction:
			{
				OutTime = Record->StartTime;
				return OutTime >= 0.0;
			}
			case EPunyEventSignalType::StopAction:
			{
				OutTime = Record->StopTime;
				return OutTime >= 0.0;
			}
			default:
			{
				return false;
			}
		}
	}

	return false;
}

UObject* FPunyInstigatorRecordCollection::GetFirstInstigator()
{
	return FirstInstigator;
}

void FPunyInstigatorRecordCollection::ClearActiveInstigators()
{
	ActiveInstigators.Empty();
}
