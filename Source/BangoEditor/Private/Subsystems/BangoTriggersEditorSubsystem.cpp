#include "BangoEditor/Subsystems/BangoTriggersEditorSubsystem.h"

#include "EditorWorldExtension.h"
#include "LevelEditor.h"
#include "BangoEditor/BangoEditorWorldExtension.h"

void UBangoTriggersEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	WorldExtension = NewObject<UBangoEditorWorldExtension>();
	
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &ThisClass::OnPostWorldInitialization);
}

void UBangoTriggersEditorSubsystem::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues InitValues)
{
	UEditorWorldExtensionManager* ExtensionManager = GEditor->GetEditorWorldExtensionsManager();

	if (!ExtensionManager)
	{
		return;
	}
	
	UEditorWorldExtensionCollection* ExtensionCollection = ExtensionManager->GetEditorWorldExtensions(World);

	ExtensionCollection->AddExtension(WorldExtension);
}
