// Copyright Quarantine Games 2021


#include "AutoLoadTutorial_Gamemode.h"
#include "../StellarFeverGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AAutoLoadTutorial_Gamemode::BeginPlay()
{
	Super::BeginPlay();

	if (UStellarFeverGameInstance* sfInstance = GetGameInstance<UStellarFeverGameInstance>())
	{
		//Load the save game for the player
		sfInstance->LoadGame();

		//Check if the tutorial is complete and open up the corresponding map
		if (sfInstance->bTutorialComplete == true)
		{
			//Set it to true so they can quit the tutorial and play the main game
			sfInstance->bTutorialComplete = true;
			UGameplayStatics::OpenLevel(this, "MainMenuMap");
		}
		else //Tutorial hasn't been completed
		{
			//Set it to true so they can quit the tutorial and play the main game
			sfInstance->bTutorialComplete = true;
			UGameplayStatics::OpenLevel(this, "TutorialMap2");
		}
	}

	else //There wasn't a Stellar Fever gameinstance at this point so we just load the main menu as normal
	{
		//Set it to true so they can quit the tutorial and play the main game
		sfInstance->bTutorialComplete = true;
		UGameplayStatics::OpenLevel(this, "MainMenuMap");
	}
}
