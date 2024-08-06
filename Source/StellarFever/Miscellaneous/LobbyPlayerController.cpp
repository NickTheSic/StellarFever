// Copyright Quarantine Games 2020


#include "LobbyPlayerController.h"
#include "LobbyGamemode.h"
#include "../MenuUI/OnlineLobbyHUD.h"
#include "../MenuUI/OnlinePlayerSelect.h"
#include "../Player/BasePlayer.h"


void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//We only want to interact with UI at first
	FInputModeUIOnly input;
	SetInputMode(input);
	bShowMouseCursor = true;

	//This should hide the Default pawn that spawns in the map
	//And make it so you can't hit the pawn while roaming
	if (APawn* pawn = GetPawn())
	{
		pawn->bOnlyRelevantToOwner = true;
		pawn->SetActorEnableCollision(false);
		pawn->SetActorHiddenInGame(true);
	}

	CheckForSelectedPlayers();
}

void ALobbyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ClientCheckPossession(InPawn);
}

void ALobbyPlayerController::ClientCheckPossession_Implementation(APawn* InPawn)
{
	if (pPlayer != nullptr || Cast<ABasePlayer>(InPawn)) //I can use this since we inherit from the other player controller
	{
		if (AOnlineLobbyHUD* hud = GetHUD<AOnlineLobbyHUD>())
		{
			UOnlinePlayerSelect* ui = hud->pPlayerSelect;
			if (ui->IsValidLowLevel())
			{
				SetToGameInput();
				ui->RemoveFromViewport();
			}
			hud->CreateDefaultPlayerHUD();
		}
	}
}

void ALobbyPlayerController::Client_DeactivateButton_Implementation(const EPlayerClass& playerClass)
{
	if (AOnlineLobbyHUD* hud = GetHUD<AOnlineLobbyHUD>())
	{
		UOnlinePlayerSelect* ui = hud->pPlayerSelect;

		//Make sure the UI is still valid and on screen
		if (ui->IsValidLowLevel() && ui->IsInViewport()) 
		{
			ui->DeactivatePlayerButton(playerClass);
		}
	}
}

void ALobbyPlayerController::ClientReactivateButton_Implementation(const EPlayerClass& InPlayerClass)
{
	if (AOnlineLobbyHUD* hud = GetHUD<AOnlineLobbyHUD>())
	{
		UOnlinePlayerSelect* ui = hud->pPlayerSelect;

		//Make sure the UI is still valid and on screen
		if (ui->IsValidLowLevel() && ui->IsInViewport()) 
		{
			ui->ReactivatePlayerButton(InPlayerClass);
		}
	}
}


void ALobbyPlayerController::NotifySelection(const EPlayerClass& playerClass)
{
	Server_NotifySelection(this, playerClass);
}


void ALobbyPlayerController::Server_NotifySelection_Implementation(APlayerController* playerController, const EPlayerClass& playerClass)
{
	if (ALobbyGamemode* lobby = GetWorld()->GetAuthGameMode<ALobbyGamemode>())
	{
		EPlayerClass selectedClass = playerClass;
		lobby->PlayerPossessNewPlayer(playerController, selectedClass, uiOnlinePlayerIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't notify lobby game mode of selection since it was a nullptr"));
	}
}


void ALobbyPlayerController::CheckForSelectedPlayers_Implementation()
{
	if (ALobbyGamemode* lobbygm = GetWorld()->GetAuthGameMode<ALobbyGamemode>())
	{
		for (EPlayerClass playerClass : lobbygm->SelectedClasses)
		{
			Client_DeactivateButton(playerClass);
		}
	}
}

void ALobbyPlayerController::Client_DisplayLoadingScreen_Implementation()
{
	if (LoadingScreenTemplate != nullptr)
	{
		UUserWidget* screen = CreateWidget<UUserWidget>(this, LoadingScreenTemplate, FName("Loading Screen"));
		screen->AddToViewport(100);
	}
}

void ALobbyPlayerController::SetToGameInput()
{
	FInputModeGameOnly game;
	SetInputMode(game);
	bShowMouseCursor = false;
}
