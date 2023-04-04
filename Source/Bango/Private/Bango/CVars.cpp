#include "Bango/CVars.h"



namespace GhostPepperGames
{
	namespace Bango
	{
		extern bool bShowEventsInGame = false;
		
		static FAutoConsoleVariableRef CVarShowEventsInGame(TEXT("Bango.ShowEventsInGame"),
			bShowEventsInGame,
			TEXT("Displays event debug info during Play In Editor"));
	}
}