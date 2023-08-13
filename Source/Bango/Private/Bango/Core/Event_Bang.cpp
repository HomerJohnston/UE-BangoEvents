#include "Bango/Core/Event_Bang.h"

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/Log.h"
#include "Bango/Core/EventSignal.h"

bool UBangoEvent_Bang::GetIsExpired()
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

EBangoActionSignalType UBangoEvent_Bang::RespondToTriggerSignal_Impl(UBangoTrigger* Trigger, FBangoEventSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoEventSignalType::ActivateEvent:
		{
			if (GetUsesActivateLimit() && GetActivateCount() >= GetActivateLimit())
			{
				return EBangoActionSignalType::None;
			}
			
			return EBangoActionSignalType::StartAction;
		}
		case EBangoEventSignalType::DeactivateEvent:
		{
			if (!bRespondToDeactivateTriggers)
			{
				UE_LOG(Bango, VeryVerbose, TEXT("UBangoEvent_Bang ignoring Deactivate trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
				return EBangoActionSignalType::None;
			}
			
			if (GetUsesActivateLimit() && GetDeactivateCount() >= GetDeactivateLimit())
			{
				return EBangoActionSignalType::None;
			}

			return EBangoActionSignalType::StopAction;
		}
		default:
		{
			UE_LOG(Bango, Warning, TEXT("UBangoEvent_Bang ignoring Unknown trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
			return EBangoActionSignalType::None;
		}
	}
}

FLinearColor UBangoEvent_Bang::GetDisplayBaseColor()
{
	return BangoColor::RedBase;
}

void UBangoEvent_Bang::ApplyColorEffects(FLinearColor& Color)
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

bool UBangoEvent_Bang::GetIsActive()
{
	return false;
}

#if WITH_EDITOR
void UBangoEvent_Bang::AppendDebugDataString_Game(TArray<FBangoDebugTextEntry>& Data)
{
	Super::AppendDebugDataString_Game(Data);
	
	const UObject* LastInstigator = GetLastActivateInstigator();

	if (IsValid(LastInstigator))
	{
		Data.Add(FBangoDebugTextEntry("Last Instigator:", FString::Printf(TEXT("%s"), *LastInstigator->GetName())));	
	}
}
#endif