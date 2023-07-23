#include "Puny/Core/EventSignal.h"

FPunyEventSignal::FPunyEventSignal() : Type(EPunyEventSignalType::None), Instigator(nullptr)
{
}

FPunyEventSignal::FPunyEventSignal(EPunyEventSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
