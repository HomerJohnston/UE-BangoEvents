#pragma once

#include "Bango/Triggers/TargetGroups/Base/BangoTargetGroup.h"

#include "BangoTargetGroup_Player.generated.h"

#define LOCTEXT_NAMESPACE "Bango"

/**
 * If your game doesn't use APawn classes for the player, or if you want to use more advanced logic to determine if
 * an actor in the trigger area is, in fact, a player, you will need to subclass this and override the behavior.
 * 
 * Bango will only use the child-most class! If you subclass this, only your subclass will be used.
 * You can override this behavior by setting bUseEvenIfInherited to true (this would be an advanced use-case).
 */
UCLASS()
class UBangoTargetGroup_Player : public UBangoTargetGroup
{
	GENERATED_BODY()
	
public:
	UBangoTargetGroup_Player()
	{
		QueryCollisionChannel = ECC_Pawn;
		
#if WITH_EDITOR
		DisplayName = LOCTEXT("BangoTargetGroup_Player_DisplayName", "Player");
		SortOrder = 30;
#endif
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

#undef LOCTEXT_NAMESPACE