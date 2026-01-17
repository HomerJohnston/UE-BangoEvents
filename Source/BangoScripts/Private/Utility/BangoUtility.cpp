#include "BangoScripts/Utility/BangoUtility.h"

#include "GameFramework/Actor.h"
#include "BangoScripts/Components/BangoActorIDComponent.h"
#include "BangoScripts/Utility/BangoLog.h"
#include "BangoScripts/EditorTooling/BangoEditorDelegates.h"

UBangoActorIDComponent* Bango::Utilities::GetActorIDComponent(AActor* Actor, bool bForceCreate)
{
	if (!Actor)
	{
		return nullptr;
	}
	
	UBangoActorIDComponent* IDComponent = nullptr;
	
	TArray<UBangoActorIDComponent*> IDComponents;
	Actor->GetComponents<UBangoActorIDComponent>(IDComponents);
	
	if (IDComponents.Num() == 0)
	{
		if (bForceCreate)
		{
			FBangoEditorDelegates::RequestNewID.Broadcast(Actor);
			Actor->GetComponents<UBangoActorIDComponent>(IDComponents);	
		}
		else
		{
			return nullptr;
		}
	}

	if (IDComponents.Num() == 1)
	{
		IDComponent = IDComponents[0];
	}
	else
	{
		UE_LOG(LogBango, Error, TEXT("Actor has more than one ID component!"));
	}
	
	return IDComponent;
}
