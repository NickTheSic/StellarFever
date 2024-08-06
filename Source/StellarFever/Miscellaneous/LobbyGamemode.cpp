// Copyright Quarantine Games 2020


#include "LobbyGamemode.h"
#include "../StellarFeverGameInstance.h"
#include "../MenuUI/OnlinePlayerSelect.h"
#include "../MenuUI/PlayerSelectPieces/PlayerSelectButton.h"
#include "../MenuUI/OnlineLobbyHUD.h"
#include "../Player/BasePlayer.h"
#include "../Player/SF_PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyPlayerController.h"


void ALobbyGamemode::BeginPlay()
{
	Super::BeginPlay();

	bUseSeamlessTravel = false; 

	//Make sure we start the Level having not previously started a game
	bIsStartingGame = false; 
}

void ALobbyGamemode::HidePreviousPawn(APlayerController* Player)
{
	if (ALobbyPlayerController* lobbyCont = Cast<ALobbyPlayerController>(Player))
	{
		lobbyCont->SetOnlinePlayerIndex(uiConnectedPlayers);
	}

	if (Player->GetPawn())
	{
		//So we can't see or run into it
		Player->GetPawn()->SetActorHiddenInGame(true);
		Player->GetPawn()->SetActorEnableCollision(false);
	}
}

void ALobbyGamemode::TellInstanceToAddClass(APlayerController* Player, const EPlayerClass& InClass)
{
	if (UStellarFeverGameInstance* sfInstance = GetGameInstance<UStellarFeverGameInstance>())
	{
		APlayerState* playerState = Player->GetPlayerState<APlayerState>();
		sfInstance->AddClassToPlayerMap(playerState, InClass);
	}
}


void ALobbyGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, "Player has Connected");

	HidePreviousPawn(NewPlayer);

	SelectedClasses.Add(EPlayerClass::Base);

	uiConnectedPlayers++;
	if (uiConnectedPlayers > uiMaxPlayerCheck)
	{	//Make sure we don't go past the max amount of playable players
		uiConnectedPlayers = uiMaxPlayerCheck;
	}

	m_ConnectedPlayers.Add(NewPlayer);

	TellInstanceToAddClass(NewPlayer, EPlayerClass::Base);

	//When the player logs in they will be notified of the currently selected classes
	TellAllOfSelectedClasses();
}


void ALobbyGamemode::Logout(AController* Exiting)
{
	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, "Player has Disconnected");

	Exiting->UnPossess();

	uiConnectedPlayers--;
	m_ConnectedPlayers.Remove(Exiting);

	if (ALobbyPlayerController* exitingLobbyCont = Cast< ALobbyPlayerController>(Exiting))
	{
		uint16 index = exitingLobbyCont->GetOnlinePlayerIndex();

		if (SelectedClasses.IsValidIndex(index))
		{
			TellAllToReactivateButton(SelectedClasses[index]);

			SelectedClasses.RemoveAt(index);
		}
	}

	GEngine->AddOnScreenDebugMessage(0, 5, FColor::Blue, "*A PLAYER HAS SUCCESSFULLY LOGGED OUT*");

	Super::Logout(Exiting);
}

void ALobbyGamemode::StartGame()
{
	if (bIsStartingGame == true) return;

	bIsStartingGame = true;

	for (AController* controller : m_ConnectedPlayers)
	{
		if(ALobbyPlayerController *player = Cast<ALobbyPlayerController>(controller) )
		{
			player->Client_DisplayLoadingScreen();
		}
	}

	UStellarFeverGameInstance* sfInstance = GetGameInstance<UStellarFeverGameInstance>();
	if (sfInstance)
	{
		sfInstance->OnlineLoadLevel(GameMap);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("The SF Instance was null while trying to start an online game"));
	}
}

void ALobbyGamemode::PlayerPossessNewPlayer(APlayerController* Player, const EPlayerClass& playerClass, uint16 playerIndex)
{
	APawn* pawn = nullptr;


	FString SearchTerm = "";


	switch (playerClass)
	{
	case EPlayerClass::Captain:
		SearchTerm.Append("Captain");
		break;
	case EPlayerClass::Engineer:
		SearchTerm.Append("Engineer");
		break;
	case EPlayerClass::Gunner:
		SearchTerm.Append("Gunner");
		break;
	case EPlayerClass::Medic:
		SearchTerm.Append("Medic");
		break;
	default:
		SearchTerm.Append("Player");
		UE_LOG(LogTemp, Error, TEXT("We couldn't find a player with the correct class tag.  Picking first unpossessed player"));
		break;
	}


	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(SearchTerm), actors);


	if (actors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actors were found! - Creating a new player to possess"));

		TSubclassOf<ABasePlayer> playerTemplate = ABasePlayer::StaticClass();

		FVector playerSpawnLocation = PlayerStartLocation;

		switch (playerClass)
		{
		case EPlayerClass::Captain:
			playerTemplate = CaptainPlayerTemplate;
			playerSpawnLocation = CaptainSpawnLocation;
			break;
		case EPlayerClass::Engineer:
			playerTemplate = EngineerPlayerTemplate;
			playerSpawnLocation = EngineerSpawnLocation;
			break;
		case EPlayerClass::Gunner:
			playerTemplate = GunnerPlayerTemplate;
			playerSpawnLocation = GunnerSpawnLocation;
			break;
		case EPlayerClass::Medic:
			playerTemplate = MedicPlayerTemplate;
			playerSpawnLocation = MedicSpawnLocation;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Incorrect player class, spawning StaticClass player"));
			break;
		}

		FActorSpawnParameters params{};
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABasePlayer* player = GetWorld()->SpawnActor<ABasePlayer>(
			playerTemplate, 
			playerSpawnLocation, 
			FRotator::ZeroRotator, 
			params);

		actors.Add(player);
	}


	for (int i = 0; i < actors.Num(); i++)
	{
		APawn* TempPawn = Cast<APawn>(actors[i]);

		if (TempPawn->IsPlayerControlled())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "This pawn is already controlled by another player, skipping");
			continue;
		}

		//We found a pawn
		pawn = TempPawn;
		break;
	}


	if (pawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find a pawn for the player to possess - LobbyGamemode PlayerPossessNewPlayer"));
		return;
	}


	if (pawn->IsPlayerControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "The pawn is already controlled by another player");
		return;
	}


	SelectedClasses[playerIndex] = playerClass;
	TellInstanceToAddClass(Player, playerClass);


	APawn* pOldPawn = Player->GetPawn();

	if (pOldPawn != nullptr)
	{
		//Because it is no longer needed
		pOldPawn->Destroy();
	}


	Player->UnPossess();
	Player->Possess(pawn);

	//UE_LOG(LogTemp, Warning, TEXT("Possessed Pawns role == %i"), pawn->GetLocalRole());

	pawn->SetOwner(Player);

	FInputModeGameOnly gameInput;
	Player->SetInputMode(gameInput);
	Player->bShowMouseCursor = false;

	if (ALobbyPlayerController* lobbyPlayer = Cast<ALobbyPlayerController>(Player))
	{
		lobbyPlayer->SetToGameInput();
	}

	TellAllOfSelectedClasses();
}


void ALobbyGamemode::TellAllOfSelectedClasses()
{
	for (AController* pc : m_ConnectedPlayers)
	{
		ALobbyPlayerController* lpc = Cast<ALobbyPlayerController>(pc);

		if (lpc->IsValidLowLevel())
		{
			for (EPlayerClass selectedClass : SelectedClasses)
			{
				lpc->Client_DeactivateButton(selectedClass);
			}
		}
	}
}


void ALobbyGamemode::TellAllToReactivateButton(const EPlayerClass& ReactivateClass)
{
	for (AController* pc : m_ConnectedPlayers)
	{
		ALobbyPlayerController* lpc = Cast<ALobbyPlayerController>(pc);

		if (lpc->IsValidLowLevel())
		{
			lpc->ClientReactivateButton(ReactivateClass);
		}
	}
}

