#pragma once

#include "BangoInitScriptActor.generated.h"

/**
 * The init script actor has two features:
 * 1) It comes with a Script Component already attached.
 * 2) It is added to the world by a menu entry which inserts it at the start of the level's actors array
 * (after AWorldSettings, so usually index [1]).
 *	  This guarantees that BeginPlay of the script has access to WorldSettings and also guarantees that this script will
 *	  start running before BeginPlay of all other actors.
 *	  
 *	  
 */
UCLASS(NotBlueprintable)
class BANGO_API ABangoInitScriptActor : public AActor
{
	GENERATED_BODY()
	
	void BeginPlay() override;
};
