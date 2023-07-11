#include "Bango/Event/BangoEvent_Bang.h"

#include "Bango/Utility/Log.h"
#include "Bango/Trigger/BangoBangTrigger.h"
#include "Bango/Action/BangoBangAction.h"
#include "Bango/Utility/BangoColor.h"
#include "VisualLogger/VisualLogger.h"

ABangoEvent_Bang::ABangoEvent_Bang()
{
}

bool ABangoEvent_Bang::ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator)
{
	if (Signal == EBangoSignal::Activate)
	{
		return Activate(NewInstigator);
	}

	UE_LOG(Bango, Warning, TEXT("Bang Event <%s> ignoring trigger signal <%s>, Bang Event not configured to repond to it"), *GetName(), *StaticEnum<EBangoSignal>()->GetValueAsString(Signal));

	return false;
}

bool ABangoEvent_Bang::HasInvalidData() const
{
	return Super::HasInvalidData();
}

bool ABangoEvent_Bang::Activate(UObject* ActivateInstigator)
{
	StartActions(ActivateInstigator);

	return true;
}

void ABangoEvent_Bang::StartActions(UObject* StartInstigator)
{
	 for (UBangoAction* Action : Actions)
	 {
	 	Action->ReceiveEventSignal(EBangoSignal::Activate, StartInstigator);
	 }
}

void ABangoEvent_Bang::ResetRemainingTriggerLimits()
{
	RemainingTriggerLimits = { EBangoSignal::Activate };
}

void ABangoEvent_Bang::UpdateProxyState()
{
	Super::UpdateProxyState();

	if (GetWorld()->IsGameWorld())
	{
		CurrentState.ClearFlag(EBangoEventState::Active);
	}
}

FLinearColor ABangoEvent_Bang::GetColorBase() const
{
	return BangoColor::BlueBase;
}

FLinearColor ABangoEvent_Bang::GetColorForProxy() const
{
	FLinearColor Color = Super::GetColorForProxy();

	if (GetWorld()->IsGameWorld())
	{
		double LastHandleDownTime = GetLastTriggerTime(EBangoSignal::Activate);
		double LastHandleUpTime = GetLastTriggerTime(EBangoSignal::Deactivate);
			
		FLinearColor ActivationColor = BangoColorOps::BrightenColor(Color);
		FLinearColor DeactivationColor = BangoColorOps::VeryDarkDesatColor(Color);
			
		double ElapsedTimeSinceLastActivation = GetWorld()->GetTimeSeconds() - LastHandleDownTime;
		double ActivationAlpha = FMath::Clamp(ElapsedTimeSinceLastActivation / 0.2, 0, 1);
			
		if (IsValid(GWorld) && (ActivationAlpha > 0))
		{
			Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
		}
		
		double ElapsedTimeSinceLastDeactivation = GetWorld()->GetTimeSeconds() - LastHandleUpTime;
		double DeactivationAlpha = FMath::Clamp(ElapsedTimeSinceLastDeactivation / (2.f * 0.2), 0, 1);
			
		if (IsValid(GWorld) && (DeactivationAlpha > 0))
		{
			Color = FMath::Lerp(DeactivationColor, Color, DeactivationAlpha);
		}
	}

	return Color;
}

TArray<FBangoDebugTextEntry> ABangoEvent_Bang::GetDebugDataString_Game() const
{
	return Super::GetDebugDataString_Game();
}

TArray<FBangoDebugTextEntry> ABangoEvent_Bang::GetDebugDataString_Editor() const
{
	return Super::GetDebugDataString_Editor();
}
