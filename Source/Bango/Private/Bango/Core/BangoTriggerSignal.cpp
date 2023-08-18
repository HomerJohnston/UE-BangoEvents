#include "Bango/Core/BangoTriggerSignal.h"


FBangoTriggerSignal::FBangoTriggerSignal() : Type(EBangoTriggerSignalType::DoNothing), Instigator(nullptr)
{
}

FBangoTriggerSignal::FBangoTriggerSignal(EBangoTriggerSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
