#include "Bango/Core/BangoEvent_Bang.h"

#include "BangoEditorTooling/BangoDebugTextEntry.h"
#include "BangoEditorTooling/BangoColors.h"
#include "Bango/Utility/BangoLog.h"
#include "Bango/Core/BangoTriggerSignal.h"
#include "Engine/World.h"

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

EBangoEventSignalType UBangoEvent_Bang::RespondToTriggerSignal_Impl(UBangoTrigger_OLD* Trigger, FBangoTriggerSignal Signal)
{
	switch (Signal.Type)
	{
		case EBangoTriggerSignalType::ActivateEvent:
		{
			if (GetUsesActivateLimit() && GetActivateCount() >= GetActivateLimit())
			{
				return EBangoEventSignalType::None;
			}
			
			return EBangoEventSignalType::EventActivated;
		}
		case EBangoTriggerSignalType::DeactivateEvent:
		{
			if (!bRespondToDeactivateTriggers)
			{
				UE_LOG(LogBango, VeryVerbose, TEXT("UBangoEvent_Bang ignoring Deactivate trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
				return EBangoEventSignalType::None;
			}
			
			if (GetUsesActivateLimit() && GetDeactivateCount() >= GetDeactivateLimit())
			{
				return EBangoEventSignalType::None;
			}

			return EBangoEventSignalType::EventDeactivated;
		}
		default:
		{
			UE_LOG(LogBango, Warning, TEXT("UBangoEvent_Bang ignoring Unknown trigger from <%s> (Bang events only respond to Activate trigger signals!"), *Signal.Instigator->GetName());
			return EBangoEventSignalType::None;
		}
	}
}

#if WITH_EDITOR
FLinearColor UBangoEvent_Bang::GetDisplayBaseColor()
{
	return Bango::Colors::RedBase;
}
#endif

#if WITH_EDITOR
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
		FLinearColor ActivationColor = Bango::Colors::Funcs::BrightenColor(Color);
		Color = FMath::Lerp(ActivationColor, Color, ActivationAlpha);
	}
	
	if (bRespondToDeactivateTriggers)
	{
		double ElapsedTimeSinceLastDeactivation = GetWorld()->GetTimeSeconds() - GetLastDeactivateTime();
		double DeactivationAlpha = FMath::Clamp(ElapsedTimeSinceLastDeactivation / 0.25, 0, 1);

		if (DeactivationAlpha > 0)
		{
			FLinearColor DeactivationColor = Bango::Colors::Funcs::DarkDesatColor(Color);
			Color = FMath::Lerp(DeactivationColor, Color, DeactivationAlpha);
		}
	}
}
#endif

#if WITH_EDITOR
bool UBangoEvent_Bang::GetIsActive()
{
	return false;
}
#endif

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