#include "Bango/Event/BangoBangEvent.h"

#include "Bango/Log.h"
#include "Bango/Trigger/BangoBangTrigger.h"
#include "Bango/Action/BangoBangAction.h"

ABangoBangEvent::ABangoBangEvent()
{
}

bool ABangoBangEvent::ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	switch (Signal)
	{
		case EBangoSignal::Activate:
		{
			return Activate(NewInstigator);
		}
		default:
		{
			UE_LOG(Bango, Warning, TEXT("Bang Event <%s> ignoring trigger signal <%s>, Bang Event will only respond to Activate signals"), *GetName(), *StaticEnum<EBangoSignal>()->GetValueAsString(Signal))
			return false;
		}
	}
}

bool ABangoBangEvent::HasInvalidData() const
{
	return Super::HasInvalidData();
}

bool ABangoBangEvent::Activate(UObject* ActivateInstigator)
{
	StartActions(ActivateInstigator);

	return true;
}

void ABangoBangEvent::StartActions(UObject* StartInstigator)
{
	 for (UBangoAction* Action : Actions)
	 {
		 Action->Start(StartInstigator);
	 }
}

void ABangoBangEvent::UpdateProxyState()
{
	Super::UpdateProxyState();

	if (GetWorld()->IsGameWorld())
	{
		CurrentState.ClearFlag(EBangoEventState::Active);
	}
}

FLinearColor ABangoBangEvent::GetColorBase() const
{
	return FColor::Red;
}

FLinearColor ABangoBangEvent::GetColorForProxy() const
{
	return Super::GetColorForProxy();
}

TArray<FBangoDebugTextEntry> ABangoBangEvent::GetDebugDataString_Game() const
{
	return Super::GetDebugDataString_Game();
}

TArray<FBangoDebugTextEntry> ABangoBangEvent::GetDebugDataString_Editor() const
{
	return Super::GetDebugDataString_Editor();
}
