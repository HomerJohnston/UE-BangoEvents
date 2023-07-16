#include "Puny/PunyTriggerSignal.h"

#include "Puny/PunyTriggerSignalType.h"

FPunyTriggerSignal::FPunyTriggerSignal() : Type(EPunyTriggerSignalType::None), Instigator(nullptr)
{
}

FPunyTriggerSignal::FPunyTriggerSignal(EPunyTriggerSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
