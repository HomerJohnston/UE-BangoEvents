#include "BangoEditorScriptClickSubsystem.h"

#include "BangoEditorSubsystem.h"
#include "Editor.h"
#include "BangoEditorTooling/BangoEditorDelegates.h"

void UBangoEditorScriptClickSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FBangoEditorDelegates::OnScriptComponentClicked.AddUObject(this, &ThisClass::OnBangoScriptComponentClicked);
}

void UBangoEditorScriptClickSubsystem::Deinitialize()
{
	FBangoEditorDelegates::OnScriptComponentClicked.RemoveAll(this);
	
	Super::Deinitialize();
}

void UBangoEditorScriptClickSubsystem::OnBangoScriptComponentClicked(UBangoScriptComponent* Component) const
{
	GEditor->SelectNone(false, true);
	GEditor->SelectActor(Component->GetOwner(), true, true, true);
	GEditor->SelectComponent(Component, true, true, true);
}
