// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Settings/BangoDevSettings.h"

#include "Bango/Utility/Log.h"

static TAutoConsoleVariable<bool> CVarBangoShowEventsInGame(
	TEXT("Bango.ShowEventsInGame"),
	false,
	TEXT("Test"));

void UBangoDevSettings::PostCDOContruct()
{
	Super::PostCDOContruct();
	
	static const auto ShowEventsInGame = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));

	ShowEventsInGame->Set(GetShowEventsInGame());

	FAutoConsoleVariableSink CVarSink(FConsoleCommandDelegate::CreateUObject(this, &ThisClass::OnCvarChange));
}

bool UBangoDevSettings::GetShowEventsInGame() const
{
	return bShowEventsInGame;
}

float UBangoDevSettings::GetFarDisplayDistance() const
{
	return FarDisplayDistance;
}

float UBangoDevSettings::GetNearDisplayDistance() const
{
	return NearDisplayDistance;
}

float UBangoDevSettings::GetEventDisplaySize() const
{
	return EventDisplaySize;
}

void UBangoDevSettings::OnCvarChange()
{	
	const IConsoleVariable* ShowInGameCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));

	if (bShowEventsInGame != ShowInGameCVar->GetBool())
	{
		UE_LOG(Bango, Display, TEXT("Updating CVar Bango.ShowEventsInGame; if you would like your setting to persist between editor startups, change it in Editor Preferences."))
		bShowEventsInGame = ShowInGameCVar->GetBool();
	}
}

#if WITH_EDITOR
void UBangoDevSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == "bShowEventsInGame")
	{
		static const auto ShowEventsInGame = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));
		ShowEventsInGame->Set(GetShowEventsInGame());
	}
}
#endif
