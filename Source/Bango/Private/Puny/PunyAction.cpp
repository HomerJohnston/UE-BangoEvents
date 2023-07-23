#include "Puny/PunyAction.h"

#include "Puny/PunyEventSignal.h"
#include "Puny/PunyEventComponent.h"

#include "Bango/Editor/BangoDebugTextEntry.h"

void UPunyAction::HandleSignal_Implementation(UPunyEvent* Event, FPunyEventSignal Signal)
{
	checkNoEntry();
}

UWorld* UPunyAction::GetWorld() const
{
	return UObject::GetWorld();
}

UPunyEventComponent* UPunyAction::GetEventComponent() const
{
	return Cast<UPunyEventComponent>(GetOuter());
}

UPunyEvent* UPunyAction::GetEvent() const
{
	return GetEventComponent()->GetEvent();
}

AActor* UPunyAction::GetActor() const
{
	return GetEventComponent()->GetOwner();
}

#if WITH_EDITOR
FText UPunyAction::GetDisplayName() const
{
	if (bUseDisplayName && !DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return GetClass()->GetDisplayNameText();
}

void UPunyAction::AppendDebugData(TArray<FBangoDebugTextEntry>& Data)
{
}

void UPunyAction::DebugDraw_Implementation(UCanvas* Canvas, APlayerController* Cont)
{
}
#endif