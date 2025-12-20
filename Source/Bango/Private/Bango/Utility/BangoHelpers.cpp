#include "Bango/Utility/BangoHelpers.h"

#include "Bango/Core/BangoScriptBlueprint.h"
#include "Bango/Core/BangoScriptContainer.h"
#include "Components/ActorComponent.h"
#include "UObject/Package.h"

#if WITH_EDITOR
TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> FBangoEditorDelegates::OnScriptContainerCreated;
TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> FBangoEditorDelegates::OnScriptContainerDestroyed;
TMulticastDelegate<void(UObject* /* Outer */, FBangoScriptContainer* /* Script Container */)> FBangoEditorDelegates::OnScriptContainerDuplicated;

TMulticastDelegate<void(FGuid /* Script ID */, UBangoScriptBlueprint*& /* Found Blueprint */)> FBangoEditorDelegates::OnBangoActorComponentUndoDelete;
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
	
	if (Component->HasAnyFlags(RF_Transient))
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
#endif