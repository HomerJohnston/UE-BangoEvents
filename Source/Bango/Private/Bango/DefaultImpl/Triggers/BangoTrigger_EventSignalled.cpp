#include "Bango/DefaultImpl/Triggers/BangoTrigger_EventSignalled.h"

#include "Algo/Count.h"
#include "Bango/Utility/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Core/BangoSignal.h"

UBangoTrigger_EventSignalled::UBangoTrigger_EventSignalled()
{
	bUseMasterEventInstigator = true;

	SignalMapping =
		{
			{ EBangoSignal::Activate, EBangoSignal::None },
			{ EBangoSignal::Deactivate, EBangoSignal::None },
		};
}

void UBangoTrigger_EventSignalled::Enable_Implementation()
{
	MasterEventMostRecentSignals.Reserve(MasterEvents.Num());
	
	for (TSoftObjectPtr<ABangoEvent> MasterEvent : MasterEvents)
	{
		if (MasterEvent.IsPending())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is not loaded!"));
			return;
		}
	
		if (!MasterEvent.IsValid())
		{
			UE_LOG(Bango, Warning, TEXT("UBangoAction_FreezeThawEvent::Execute called, but the target event is invalid!"));
			return;
		}

		MasterEvent->OnEventTriggered.AddDynamic(this, &ThisClass::OnTargetEventSignalled);

		MasterEventMostRecentSignals.Add(EBangoSignal::None);
	}
}

void UBangoTrigger_EventSignalled::Disable_Implementation()
{
	for (TSoftObjectPtr<ABangoEvent> MasterEvent : MasterEvents)
	{
		MasterEvent->OnEventTriggered.RemoveDynamic(this, &ThisClass::OnTargetEventSignalled);
	}
}

void UBangoTrigger_EventSignalled::OnTargetEventSignalled(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator)
{
	int32 Index = MasterEvents.Find(Event);

	if (Index == INDEX_NONE)
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_EventSignalled::OnTargetEventSignalled was called with an event that was not set in its list!"));
		return;
	}

	check(MasterEventMostRecentSignals.IsValidIndex(Index));
	check(MasterEventMostRecentSignals.Num() == MasterEvents.Num());
	
	MasterEventMostRecentSignals[Index] = Signal;
	
	bool bPerformSignal = true;
	
	switch(Requirement)
	{
		case (EBangoEventSignalledRequirement::AnyChanged):
		{
			bPerformSignal = true;
			break;
		}
		case (EBangoEventSignalledRequirement::AllChanged):
		{
			bPerformSignal = Algo::Count(MasterEventMostRecentSignals, Signal) == MasterEventMostRecentSignals.Num();
			break;
		}
		case (EBangoEventSignalledRequirement::AllActivateAnyDeactivate):
		{
			if (Signal == EBangoSignal::Activate)
			{
				bPerformSignal = Algo::Count(MasterEventMostRecentSignals, Signal) == MasterEventMostRecentSignals.Num();
				break;
			}
			else if (Signal == EBangoSignal::Deactivate)
			{
				bPerformSignal = Algo::Count(MasterEventMostRecentSignals, Signal) >= 1;
				break;
			}
			checkNoEntry();
		}
		case (EBangoEventSignalledRequirement::AnyActivateAllDeactivate):
		{
			if (Signal == EBangoSignal::Activate)
			{
				bPerformSignal = Algo::Count(MasterEventMostRecentSignals, Signal) >= 1;
			}
			else if (Signal == EBangoSignal::Deactivate)
			{
				bPerformSignal = Algo::Count(MasterEventMostRecentSignals, Signal) == MasterEventMostRecentSignals.Num();
			}
			checkNoEntry();
		}
		default:
		{
			break;
		}
	}

	if (!bPerformSignal)
	{
		return;
	}
	
	if (bOverrideSignalMapping)
	{
		EBangoSignal* SignalToSend = SignalMapping.Find(Signal);

		if (SignalToSend)
		{
			SendTriggerSignal(*SignalToSend, SignalInstigator);
		}
		else
		{
			UE_LOG(Bango, Warning, TEXT("UBangoTrigger_SlaveToEvent failed to process signal <%s>, missing from signal mapping settings"), *StaticEnum<EBangoSignal>()->GetValueAsString(Signal));
		}
	}
	else
	{
		SendTriggerSignal(Signal, SignalInstigator);
	}
}
