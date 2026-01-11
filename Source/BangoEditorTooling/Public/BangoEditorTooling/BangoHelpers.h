#pragma once

class UActorComponent;

enum class EBangoAllowInvalid : uint8
{
	AllowInvalid,
	RequireValid
};

using enum EBangoAllowInvalid;

#if WITH_EDITOR
namespace Bango::Editor
{	
	BANGOEDITORTOOLING_API bool IsComponentInEditedLevel(UActorComponent* Component, EBangoAllowInvalid AllowInvalid = EBangoAllowInvalid::RequireValid);
	
	// BANGOEDITORTOOLING_API bool IsComponentBeingDeleted(UActorComponent* Component);
	
	// BANGO_API UBangoActorIDComponent* GetActorIDComponent(AActor* Actor, bool bForceCreate = false);
	
	// BANGO_API FName GetBangoName(AActor* Actor);
}
#endif