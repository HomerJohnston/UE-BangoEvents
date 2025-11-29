#include "BangoEditor/BangoEditorWorldExtension.h"

#include "Bango/Triggers/BangoTrigger.h"
#include "BangoEditor/Windows/BangoTriggerViewportPopup.h"
#include "Subsystems/EditorActorSubsystem.h"

void UBangoEditorWorldExtension::Init()
{
	SetActive(true);
}

bool UBangoEditorWorldExtension::InputKey(FEditorViewportClient* InViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (Event == IE_DoubleClick)
	{
	
		UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		TArray<AActor*> SelectedActors;
		
		if (!EditorActorSubsystem)
		{
			return false;
		}

		SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
		
		if (SelectedActors.Num() != 1)
		{
			return false;
		}

		AActor* Actor = SelectedActors[0];

		if (ABangoTrigger* TriggerActor = Cast<ABangoTrigger>(Actor))
		{
			FBangoTriggerViewportPopup::Show(TriggerActor);
			
			UE_LOG(LogTemp, Display, TEXT("Test2"));
			
			return true;
		}
	}

	return false;
}
