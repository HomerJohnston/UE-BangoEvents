// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Action/BangoAction.h"

#include "Bango/Utility/Log.h"
#include "Bango/Event/BangoEvent.h"

void UBangoAction::ReceiveEventSignal_Implementation(EBangoSignal Signal, UObject* SignalInstigator)
{
}

UWorld* UBangoAction::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return GetOuter()->GetWorld();
	}
	else
	{
		return nullptr;		
	}
}

ABangoEvent* UBangoAction::GetEvent() const
{
	return Cast<ABangoEvent>(GetOuter());
}

void UBangoAction::GetDebugDataString(TArray<FBangoDebugTextEntry>& Data)
{
}

#if WITH_EDITOR
void UBangoAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif

FText UBangoAction::GetDisplayName_Implementation()
{
	if (DisplayName.IsEmpty())
	{
#if WITH_EDITOR
		return GetClass()->GetDisplayNameText();
#else
		return FText::FromString(GetClass()->GetName());
#endif
	}
	
	return DisplayName;
}
