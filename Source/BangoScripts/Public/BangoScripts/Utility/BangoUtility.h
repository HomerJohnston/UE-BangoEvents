#pragma once

#include "BangoScripts/BangoScripts.h"

class AActor;
class UBangoActorIDComponent;

namespace Bango::Utilities
{
	BANGOSCRIPTS_API UBangoActorIDComponent* GetActorIDComponent(AActor* Actor, bool bForceCreate = false);
}