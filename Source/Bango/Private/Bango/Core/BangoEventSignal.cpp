#include "Bango/Core/BangoEventSignal.h"

FBangoEventSignal::FBangoEventSignal() : Type(EBangoEventSignalType::None), Instigator(nullptr)
{
}

FBangoEventSignal::FBangoEventSignal(EBangoEventSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
