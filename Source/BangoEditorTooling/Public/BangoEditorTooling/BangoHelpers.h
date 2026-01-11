#pragma once

class UActorComponent;

#if WITH_EDITOR
namespace Bango::Editor
{	
	BANGOEDITORTOOLING_API bool IsComponentInEditedLevel(UActorComponent* Component);
	
	BANGOEDITORTOOLING_API bool IsBeingEditorDeleted(UActorComponent* Component);
	
	// BANGO_API UBangoActorIDComponent* GetActorIDComponent(AActor* Actor, bool bForceCreate = false);
	
	// BANGO_API FName GetBangoName(AActor* Actor);
}
#endif