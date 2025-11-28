#pragma once

#include "Bango/Triggers/Targets/Base/BangoTargetBase.h"

#include "BangoTarget_Player.generated.h"

/**
 * If your game doesn't use APawn classes for the player, or if you want to use more advanced logic to determine if
 * an actor in the trigger area is, in fact, a player, you will need to subclass this and override the behavior.
 * 
 * Bango will only use the child-most class! If you subclass this, only your subclass will be used.
 * You can override this behavior by setting bUseEvenIfInherited to true (this would be an advanced use-case).
 */
UCLASS()
class UBangoTarget_Player : public UBangoTargetBase
{
	GENERATED_BODY()
	
public:
	UBangoTarget_Player()
	{
		PresenceCollisionChannel = ECC_Pawn;
	}
	
	bool IsTarget(AActor* Candidate) override
	{
		if (APawn* Pawn = Cast<APawn>(Candidate))
		{
			return Pawn->IsPlayerControlled();
		}
		
		return false;
	}
};