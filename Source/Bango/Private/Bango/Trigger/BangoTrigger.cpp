// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Trigger/BangoTrigger.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Log.h"


ABangoEvent* UBangoTrigger::GetEventBP()
{
	return Cast<ABangoEvent>(GetOuter());
}

ABangoEvent* UBangoTrigger::GetEvent()
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoTrigger::SetEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

void UBangoTrigger::Enable_Implementation()
{
	UE_LOG(Bango, Warning, TEXT("Trigger %s does not have any enable logic!"), *GetClass()->GetName());
}

void UBangoTrigger::Disable_Implementation()
{
	UE_LOG(Bango, Warning, TEXT("Trigger %s does not have any disable logic!"), *GetClass()->GetName());
}

#if WITH_EDITOR
void UBangoTrigger::SendTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	if (Signal == EBangoSignal::None) { return; }
	
	TriggerSignal.Broadcast(Signal, NewInstigator);
}

FText UBangoTrigger::GetDisplayName()
{
	return GetClass()->GetDisplayNameText();
}
#endif
