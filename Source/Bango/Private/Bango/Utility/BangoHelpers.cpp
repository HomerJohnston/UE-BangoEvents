#include "Bango/Utility/BangoHelpers.h"

#include "Bango/Components/BangoActorIDComponent.h"
#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScriptContainer.h"
#include "Bango/Utility/BangoLog.h"
#include "Components/ActorComponent.h"
#include "UObject/Package.h"

#if WITH_EDITOR
TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> FBangoEditorDelegates::OnScriptContainerCreated;
TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> FBangoEditorDelegates::OnScriptContainerDestroyed;
TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> FBangoEditorDelegates::OnScriptContainerDuplicated;

TMulticastDelegate<void(FGuid /* Script ID */, UBangoScriptBlueprint*& /* Found Blueprint */)> FBangoEditorDelegates::OnBangoActorComponentUndoDelete;

TMulticastDelegate<void(AActor* Actor)> FBangoEditorDelegates::RequestNewID;
#endif

#if WITH_EDITOR
bool Bango::IsComponentInEditedLevel(UActorComponent* Component)
{
	if (!GEditor)
		return false;
		
	if (GIsPlayInEditorWorld)
		return false;
	
	UWorld* World = Component->GetWorld();
	
	if (!World)
		return false;

	if (World->bIsTearingDown)
	{
		return false;
	}
	
	if (Component->HasAnyFlags(RF_Transient | RF_ClassDefaultObject | RF_DefaultSubObject))
		return false;
	
	if (Component->GetPackage() == GetTransientPackage())
		return false;
		
	if (Component->IsDefaultSubobject())
	{
		if (World->IsPlayInEditor())
			return false;
		
		return true;
	}
	else
	{
		ULevel* Level = Component->GetComponentLevel();
	
		if (!Level)
			return false;
	
		if (Level->GetPackage() == GetTransientPackage())
			return false;
	
		if (Component->GetComponentLevel() == nullptr)
			return false;
	
		return true;
	}
}

UBangoActorIDComponent* Bango::GetActorIDComponent(AActor* Actor, bool bForceCreate)
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

FName Bango::GetBangoName(AActor* Actor)
{
	UBangoActorIDComponent* IDComponent = GetActorIDComponent(Actor);
	
	return IDComponent ? IDComponent->GetBangoName() : NAME_None;
}
#endif
