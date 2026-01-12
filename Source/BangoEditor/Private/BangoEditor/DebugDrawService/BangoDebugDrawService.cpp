#include "BangoDebugDrawService.h"

#include "Editor.h"
#include "BangoEditorTooling/BangoDebugDrawCanvas.h"
#include "BangoEditorTooling/BangoEditorDelegates.h"
#include "Debug/DebugDrawService.h"

void UBangoDebugDrawService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UDebugDrawService::Register(TEXT("BangoEventsShowFlag"), FDebugDrawDelegate::CreateUObject(this, &ThisClass::DebugDraw));
}

void UBangoDebugDrawService::DebugDraw(UCanvas* Canvas, APlayerController* ALWAYSNULL_DONOTUSE) const
{
	FBangoDebugDrawCanvas Data(Canvas);
	
	FBangoEditorDelegates::BangoDebugDraw.Broadcast(Data, GEditor->IsPlayingSessionInEditor());
}
