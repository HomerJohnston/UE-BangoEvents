#include "BangoActorIDEditorSubsystem.h"

#include "Menus/BangoEditorMenus.h"
#include "BangoScripts/EditorTooling/BangoEditorDelegates.h"
#include "BangoScripts/EditorTooling/BangoEditorLog.h"
#include "GameFramework/Actor.h"

// ----------------------------------------------

void UBangoActorIDEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FBangoEditorDelegates::RequestNewID.AddUObject(this, &ThisClass::OnRequestNewID);
}

// ----------------------------------------------

void UBangoActorIDEditorSubsystem::Deinitialize()
{
	FBangoEditorDelegates::RequestNewID.RemoveAll(this);
	
	Super::Deinitialize();
}

// ----------------------------------------------

void UBangoActorIDEditorSubsystem::OnRequestNewID(AActor* Actor) const
{
	UE_LOG(LogBangoEditor, Verbose, TEXT("OnRequestNewID: %s"), *Actor->GetName());
	FBangoEditorMenus::SetEditActorID(Actor, true);
}

// ----------------------------------------------
