#include "Puny/PunyEventSignal.h"

#include "Puny/PunyEventSignalType.h"

FPunyEventSignal::FPunyEventSignal() : Type(EPunyEventSignalType::None), Instigator(nullptr)
{
}

FPunyEventSignal::FPunyEventSignal(EPunyEventSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
