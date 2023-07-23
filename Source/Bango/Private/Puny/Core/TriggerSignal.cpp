#include "Puny/Core/TriggerSignal.h"


FPunyTriggerSignal::FPunyTriggerSignal() : Type(EPunyTriggerSignalType::None), Instigator(nullptr)
{
}

FPunyTriggerSignal::FPunyTriggerSignal(EPunyTriggerSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
