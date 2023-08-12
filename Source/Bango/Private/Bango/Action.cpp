#include "Bango/Action.h"

#include "Bango/Core/EventSignal.h"
#include "Bango/Core/EventComponent.h"

#include "Bango/Editor/BangoDebugTextEntry.h"

void UBangoAction::HandleSignal_Implementation(UBangoEvent* Event, FBangoEventSignal Signal)
{
	checkNoEntry();
}

UWorld* UBangoAction::GetWorld() const
{
	return UObject::GetWorld();
}

UBangoEventComponent* UBangoAction::GetEventComponent() const
{
	return Cast<UBangoEventComponent>(GetOuter());
}

UBangoEvent* UBangoAction::GetEvent() const
{
	return GetEventComponent()->GetEvent();
}

AActor* UBangoAction::GetActor() const
{
	return GetEventComponent()->GetOwner();
}

#if WITH_EDITOR
FText UBangoAction::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}

void UBangoAction::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
}

bool UBangoAction::HasValidSetup()
{
	return true;
}

void UBangoAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif