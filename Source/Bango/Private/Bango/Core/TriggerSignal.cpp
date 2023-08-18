#include "Bango/Core/TriggerSignal.h"


FBangoTriggerSignal::FBangoTriggerSignal() : Type(EBangoTriggerSignalType::None), Instigator(nullptr)
{
}

FBangoTriggerSignal::FBangoTriggerSignal(EBangoTriggerSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
