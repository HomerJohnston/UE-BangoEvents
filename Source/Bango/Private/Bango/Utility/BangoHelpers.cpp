#include "Bango/Utility/BangoHelpers.h"

#include "Components/ActorComponent.h"
#include "UObject/Package.h"

#if WITH_EDITOR
TMulticastDelegate<void(UBangoScriptComponent*)> FBangoEditorDelegates::OnScriptComponentCreated;
TMulticastDelegate<void(UBangoScriptComponent*)> FBangoEditorDelegates::OnScriptComponentDestroyed;
#endif

#if WITH_EDITOR
bool Bango::IsComponentInEditedLevel(UActorComponent* Component)
{
	if (!GEditor)
		return false;
		
	if (GIsPlayInEditorWorld)
		return false;
	
	if (!Component->GetWorld())
		return false;

	if (Component->HasAnyFlags(RF_Transient))
		return false;
	
	if (Component->GetPackage() == GetTransientPackage())
		return false;
		
	if (Component->IsDefaultSubobject())
	{
		if (Component->GetWorld()->IsPlayInEditor())
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