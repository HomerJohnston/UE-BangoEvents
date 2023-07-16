#include "Bango/DefaultImpl/Triggers/BangoTrigger_EventTriggered.h"

#include "Algo/Count.h"
#include "Bango/Utility/Log.h"
#include "Bango/Event/BangoEvent.h"
#include "Bango/Core/BangoSignal.h"

UBangoTrigger_EventTriggered::UBangoTrigger_EventTriggered()
{
	bUseMasterEventInstigator = true;

	SignalMapping =
		{
			{ EBangoSignal::Activate, EBangoSignal::None },
			{ EBangoSignal::Deactivate, EBangoSignal::None },
		};
}

void UBangoTrigger_EventTriggered::Enable_Implementation()
{
	MasterEventMostRecentSignals.Reserve(FMath::Max(MasterEvents.Num(), 1));

	if (!bWatchOtherEvents)
	{
		MasterEvents.Empty();
		MasterEvents.Add(GetEvent());
	}

	if (MasterEvents.Num() == 0)
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_EventTriggered on event %s is not watching any events!"), *GetEvent()->GetName());
	}
	
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

		MasterEvent->OnEventTriggered.AddDynamic(this, &ThisClass::OnTargetEventTriggered);
		MasterEventMostRecentSignals.Add(EBangoSignal::None);
	}
}

void UBangoTrigger_EventTriggered::Disable_Implementation()
{
	for (TSoftObjectPtr<ABangoEvent> MasterEvent : MasterEvents)
	{
		MasterEvent->OnEventTriggered.RemoveDynamic(this, &ThisClass::OnTargetEventTriggered);
	}

	MasterEventMostRecentSignals.Empty();
}

void UBangoTrigger_EventTriggered::OnTargetEventTriggered(ABangoEvent* Event, EBangoSignal Signal, UObject* SignalInstigator)
{
	int32 Index = MasterEvents.Find(Event);

	if (Index == INDEX_NONE)
	{
		UE_LOG(Bango, Error, TEXT("UBangoTrigger_EventTriggered::OnTargetEventTriggered was called with an event that was not set in its list!"));
		return;
	}

	check(MasterEventMostRecentSignals.IsValidIndex(Index));
	check(MasterEventMostRecentSignals.Num() == MasterEvents.Num());
	
	MasterEventMostRecentSignals[Index] = Signal;
	
	bool bPerformSignal = true;
	
	switch(Requirement)
	{
		case (EBangoEventTriggeredRequirement::AnyChanged):
		{
			bPerformSignal = true;
			break;
		}
		case (EBangoEventTriggeredRequirement::AllChanged):
		{
			bPerformSignal = Algo::Count(MasterEventMostRecentSignals, Signal) == MasterEventMostRecentSignals.Num();
			break;
		}
		case (EBangoEventTriggeredRequirement::AllActivateAnyDeactivate):
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
		case (EBangoEventTriggeredRequirement::AnyActivateAllDeactivate):
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
