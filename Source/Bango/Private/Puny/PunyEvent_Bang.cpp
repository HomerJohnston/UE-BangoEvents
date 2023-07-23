#include "Puny/PunyEvent_Bang.h"

#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/Log.h"
#include "Puny/PunyEventSignalType.h"
#include "Puny/PunyTriggerSignal.h"
#include "Puny/PunyTriggerSignalType.h"

bool UPunyEvent_Bang::GetIsExpired()
{
	if (!GetUsesActivateLimit())
	{
		return false;
	}

	bool bExpired = GetActivateCount() >= GetActivateLimit();

	if (bExpired && bRespondToDeactivateTriggers)
	{
		bExpired = GetDeactivateCount() >= GetDeactivateLimit();
	}

	return bExpired;
}

EPunyEventSignalType UPunyEvent_Bang::RespondToTriggerSignal_Impl(UPunyTrigger* Trigger, FPunyTriggerSignal Signal)
{
	switch (Signal.Type)
	{
		case EPunyTriggerSignalType::ActivateEvent:
		{
			if (GetUsesActivateLimit() && GetActivateCount() >= GetActivateLimit())
			{
				return EPunyEventSignalType::None;
			}
			
			return EPunyEventSignalType::StartAction;
		}
		case EPunyTriggerSignalType::DeactivateEvent:
		{
			if (!bRespondToDeactivateTriggers)
			{
				UE_LOG(Bango, Warning, TEXT("UPunyEvent_Bang ignoring Deactivate trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
				return EPunyEventSignalType::None;
			}
			
			if (GetUsesActivateLimit() && GetDeactivateCount() >= GetDeactivateLimit())
			{
				return EPunyEventSignalType::None;
			}

			return EPunyEventSignalType::StopAction;
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

	double ElapsedTimeSinceLastActivation = GetWorld()->GetTimeSeconds() - GetLastActivateTime();
	double ActivationAlpha = FMath::Clamp(ElapsedTimeSinceLastActivation / 0.25, 0, 1);
			
	if (ActivationAlpha > 0)
	{
		FLinearColor ActivationColor = BangoColorOps::BrightenColor(Color);
		Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
	}
	
	if (bRespondToDeactivateTriggers)
	{
		double ElapsedTimeSinceLastDeactivation = GetWorld()->GetTimeSeconds() - GetLastDeactivateTime();
		double DeactivationAlpha = FMath::Clamp(ElapsedTimeSinceLastDeactivation / 0.25, 0, 1);

		if (DeactivationAlpha > 0)
		{
			FLinearColor DeactivationColor = BangoColorOps::DarkDesatColor(Color);
			Color = FMath::Lerp(DeactivationColor, Color, DeactivationAlpha);
		}
	}
}

bool UPunyEvent_Bang::GetIsPlungerPushed()
{
	double WorldTime = GetWorld()->GetTimeSeconds();
	
	bool ActivatePushed = (WorldTime - GetLastActivateTime()) <= 0.25f;

	bool DeactivatePushed = (bRespondToDeactivateTriggers) ? (WorldTime - GetLastDeactivateTime() <= 0.25f) : false;

	return ActivatePushed || DeactivatePushed;
}
