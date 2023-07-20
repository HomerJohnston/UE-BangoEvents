#include "Puny/PunyEvent_Bang.h"

#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/Log.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

EPunyEventSignalType UPunyEvent_Bang::RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	switch (Signal.Type)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{
			return EPunyEventSignalType::StartAction;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			UE_LOG(Bango, Warning, TEXT("UPunyEvent_Bang ignoring Deactivate trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
			return EPunyEventSignalType::None;
		}
		default:
		{
			UE_LOG(Bango, Warning, TEXT("UPunyEvent_Bang ignoring Unknown trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
			return EPunyEventSignalType::None;
		}
	}
}

FLinearColor UPunyEvent_Bang::GetDisplayBaseColor()
{
	return BangoColor::RedBase;
}

void UPunyEvent_Bang::ApplyColorEffects(FLinearColor& Color)
{
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}

	double LastHandleDownTime = GetLastActivateTime();
		
	FLinearColor ActivationColor = BangoColorOps::BrightenColor(Color);
		
	double ElapsedTimeSinceLastActivation = GetWorld()->GetTimeSeconds() - LastHandleDownTime;
	double ActivationAlpha = FMath::Clamp(ElapsedTimeSinceLastActivation / 0.25, 0, 1);
		
	if (ActivationAlpha > 0)
	{
		Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
	}
}

bool UPunyEvent_Bang::GetIsPlungerPushed()
{
	return (GetWorld()->GetTimeSeconds() - GetLastActivateTime() <= 0.25f);
}
