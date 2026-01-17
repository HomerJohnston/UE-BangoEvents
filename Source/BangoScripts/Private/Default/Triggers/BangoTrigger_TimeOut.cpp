#include "BangoScripts/Default/Triggers/BangoTrigger_TimeOut.h"

UBangoTrigger_TimeOut::UBangoTrigger_TimeOut()
{
}

void UBangoTrigger_TimeOut::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void UBangoTrigger_TimeOut::Disable_Implementation()
{
	Super::Disable_Implementation();
}

#if WITH_EDITOR
bool UBangoTrigger_TimeOut::HasValidSetup()
{
	return Super::HasValidSetup();
}
#endif

#if WITH_EDITOR
void UBangoTrigger_TimeOut::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
	Super::AppendDebugData(Data);
}
#endif