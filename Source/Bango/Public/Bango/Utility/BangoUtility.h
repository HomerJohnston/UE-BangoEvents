#pragma once

class UBangoActorIDComponent;

namespace Bango::Utilities
{
	BANGO_API UBangoActorIDComponent* GetActorIDComponent(AActor* Actor, bool bForceCreate = false);
}