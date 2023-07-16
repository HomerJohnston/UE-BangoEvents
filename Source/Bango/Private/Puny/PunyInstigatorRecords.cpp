#include "Puny/PunyInstigatorRecords.h"

#include "Puny/PunyTriggerSignalType.h"

FPunyInstigatorRecord::FPunyInstigatorRecord() : SignalType(EPunyTriggerSignalType::None), Time(-1.0)
{
}

FPunyInstigatorRecord::FPunyInstigatorRecord(EPunyTriggerSignalType InSignalType, double InTime) : SignalType(InSignalType), Time(InTime)
{
}

FPunyInstigatorRecords::FPunyInstigatorRecords() : Instigator(nullptr)
{
}

FPunyInstigatorRecords::FPunyInstigatorRecords(UObject* InInstigator) : Instigator(InInstigator)
{
}
