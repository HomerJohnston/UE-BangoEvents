#include "Bango/Core/InstigatorRecords.h"

#include "Bango/Core/EventSignal.h"

TMap<UObject*, FBangoInstigatorRecord> FBangoInstigatorRecordCollection::GetData()
{ return AllInstigatorRecords; }

FBangoInstigatorRecord* FBangoInstigatorRecordCollection::FindRecordFor(UObject* PotentialInstigator)
{
	return (AllInstigatorRecords.Find(PotentialInstigator));
}

bool FBangoInstigatorRecordCollection::IsInstigatorActive(UObject* Instigator)
{
	if (!IsValid(Instigator))
	{
		return false;
	}
	
	return ActiveInstigators.Contains(Instigator);
}

int32 FBangoInstigatorRecordCollection::GetNumActiveInstigators()
{
	return ActiveInstigators.Num();
}

void FBangoInstigatorRecordCollection::UpdateInstigatorRecord(UObject* Instigator, EBangoEventSignalType SignalType, double Time)
{
	auto& Record = AllInstigatorRecords.FindOrAdd(Instigator);
	Record.Instigator = Instigator;

	switch (SignalType)
	{
		case EBangoEventSignalType::EventActivated:
		{
			Record.StartTime = Time;
			
			if (ActiveInstigators.Num() == 0)
			{
				FirstInstigator = Instigator;
			}

			ActiveInstigators.Add(Instigator);
			
			break;
		}
		case EBangoEventSignalType::EventDeactivated:
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

bool FBangoInstigatorRecordCollection::GetInstigationTime(UObject* Instigator, EBangoEventSignalType SignalType, double& OutTime)
{
	if (FBangoInstigatorRecord* Record = AllInstigatorRecords.Find(Instigator))
	{
		switch (SignalType)
		{
			case EBangoEventSignalType::EventActivated:
			{
				OutTime = Record->StartTime;
				return OutTime >= 0.0;
			}
			case EBangoEventSignalType::EventDeactivated:
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

UObject* FBangoInstigatorRecordCollection::GetFirstInstigator()
{
	return FirstInstigator;
}

void FBangoInstigatorRecordCollection::ClearActiveInstigators()
{
	ActiveInstigators.Empty();
}
