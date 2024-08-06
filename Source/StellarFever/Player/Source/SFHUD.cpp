// Copyright Quarantine Games 2020


#include "../SFHUD.h"
#include "../BasePlayer.h"
#include "../PlayerHUDWidget.h"
#include "../../MenuUI/PlayerSelectMenu.h"
#include "../../PauseMenu/EndGameMenu.h"
#include "../../PauseMenu/PauseMenu.h"
#include "Components/Button.h"


ASFHUD::ASFHUD()
{
	bAutomaticallyCreatePlayerHUD = true;
}

void ASFHUD::BeginPlay()
{
	Super::BeginPlay();

	if (bAutomaticallyCreatePlayerHUD)
	{
		CreateDefaultPlayerHUD();
	}

	CreatePauseMenu();
}


void ASFHUD::OpenPauseMenu()
{
	if (!pPauseMenu->IsValidLowLevel())
	{
		CreatePauseMenu();
		UE_LOG(LogTemp, Warning, TEXT("The pause menu was null, creating a new one"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The pause menu was not null, reusing old menu"))
	}

	if (pPauseMenu)
	{
		if (!pPauseMenu->IsInViewport())
		{
			pPauseMenu->AddToViewport(9998);
			pPauseMenu->SwitchToMainMenu();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("The pause menu was already in the viewport "))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The pause menu was null while trying to open"));
	}

}

void ASFHUD::ClosePauseMenu()
{
	if (pPauseMenu->IsValidLowLevel())
	{
		if (pPauseMenu->IsInViewport())
		{
			pPauseMenu->RemoveFromParent();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("The pause menu wasn't in the viewport and trying to remove - SFHUD"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The pause menu was null while trying to close - SFHUD"));
	}
}


void ASFHUD::CreatePauseMenu()
{
	if (pPauseMenuTemplate)
	{
		pPauseMenu = CreateWidget<UPauseMenu>(GetOwningPlayerController(), pPauseMenuTemplate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The pause menu template was null - SFHUD"))
	}
}

void ASFHUD::CreateDefaultPlayerHUD()
{
	if (pPlayerUITemplate != nullptr)
	{
		//Do we want to use GetWorld here or something different?
		pPlayerUI = CreateWidget<UPlayerHUDWidget>(GetWorld(), pPlayerUITemplate);
		pPlayerUI->AddToViewport();

		if (ABasePlayer* player = Cast<ABasePlayer>(GetOwningPawn()))
		{
			pPlayerUI->SetOwningBasePlayer(player);
			pPlayerUI->SetPlayerTintColour(player->GetPlayerInformation().ClassColour);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to get the player character on Begin Play - SFHUD"));
		}
	}
}

void ASFHUD::OpenEndGameMenu()
{
	if (!ensure(pEndGameMenuTemplate)) return;

	pEndGameMenu = CreateWidget<UEndGameMenu>(GetOwningPlayerController(), pEndGameMenuTemplate);
	pEndGameMenu->AddToViewport(9000);
}


void ASFHUD::SetEndGameInformation(const EEndGameReason& reason)
{
	if (pEndGameMenu)
	{
		pEndGameMenu->SetReasonForEndingGame(reason);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("The end game menu was null while trying to set the end game information"));
	}
}



void ASFHUD::OpenPlayerSelectMenu()
{
	if (pEndGameMenu && pEndGameMenu->IsInViewport())
	{
		pEndGameMenu->RemoveFromViewport();
	}

	if (pPlayerSelectMenuTemplate != nullptr)
	{
		pPlayerSelectMenu = CreateWidget<UPlayerSelectMenu>(GetOwningPlayerController(), pPlayerSelectMenuTemplate, FName("End Game Player Select"));
		
		//Remove and hide the back button since we are using the main menu player select
		pPlayerSelectMenu->BackButton->SetIsEnabled(false);
		pPlayerSelectMenu->BackButton->RemoveFromParent();

		pPlayerSelectMenu->AddToViewport(9001);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The player select menu template was null so we are opening the lobby map instead"));
	}

}
