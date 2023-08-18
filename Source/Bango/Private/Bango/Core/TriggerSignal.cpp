#include "Bango/Core/BangoTriggerSignal.h"


FBangoTriggerSignal::FBangoTriggerSignal() : Type(EBangoTriggerSignalType::None), Instigator(nullptr)
{
}

FBangoTriggerSignal::FBangoTriggerSignal(EBangoTriggerSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
