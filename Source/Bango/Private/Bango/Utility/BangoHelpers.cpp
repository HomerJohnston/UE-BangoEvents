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

bool Bango::IsBeingEditorDeleted(UActorComponent* Component)
{
	// 1. Must be in editor world
	if (GIsPlayInEditorWorld || Component->GetWorld() == nullptr || Component->GetWorld()->IsGameWorld())
		return false;

	// 2. Component must be explicitely removed from its owner
	if (Component->GetOwner() && !Component->GetOwner()->GetComponents().Contains(Component))
		return true;

	// 3. NOT deleted because PIE ended
	if (GEditor && GEditor->PlayWorld != nullptr)
		return false;

	// 4. NOT deleted because a Blueprint reinstance/recompile replaced it
	if (Component->HasAnyFlags(RF_Transient) && Component->HasAnyFlags(RF_ClassDefaultObject) == false)
		return false;

	return false;
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
