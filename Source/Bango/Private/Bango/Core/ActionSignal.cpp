#include "Bango/Core/ActionSignal.h"

FBangoActionSignal::FBangoActionSignal() : Type(EBangoActionSignalType::None), Instigator(nullptr)
{
}

FBangoActionSignal::FBangoActionSignal(EBangoActionSignalType InType, UObject* InInstigator) : Type(InType), Instigator(InInstigator)
{
}
