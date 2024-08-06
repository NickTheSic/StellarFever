// Copyright Quarantine Games 2020


#include "StellarFeverMultiplayerGameMode.h"
#include "../StellarFeverGameInstance.h"
#include "../Player/BasePlayer.h"
#include "../Player/CaptainPlayer.h"
#include "../Player/EngineerPlayer.h"
#include "../Player/GunnerPlayer.h"
#include "../Player/MedicPlayer.h"
#include "../Player/SF_PlayerController.h"
#include "../Player/SF_PlayerState.h"



APlayerController* AStellarFeverMultiplayerGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UStellarFeverGameInstance* sfInstance = GetGameInstance<UStellarFeverGameInstance>();

	if (sfInstance != nullptr && sfInstance->OnlineSelectedPlayerMap.Contains(UniqueId))
	{
		GetPlayerClassFromType(sfInstance->OnlineSelectedPlayerMap[UniqueId]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("There was no valid player selected for this player: "));

		if (AvailableClasses.Num() > 0) //Will select Medic - Gunner - Engineer - Captain in that order if unselected
		{
			UE_LOG(LogTemp, Error, TEXT("Getting unselected class from end of array"));
			GetPlayerClassFromType(AvailableClasses.Last());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("All player types were spawned, spawning random"));

			EPlayerClass rand = (EPlayerClass)(FMath::RandRange(0, 3));
			GetPlayerClassFromType(rand);
		}
	}

	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AStellarFeverMultiplayerGameMode::GetPlayerClassFromType(const EPlayerClass& InClass)
{
	switch (InClass)
	{
	case EPlayerClass::Captain:
		DefaultPawnClass = CaptainPlayerTemplate;
		break;
	case EPlayerClass::Engineer:
		DefaultPawnClass = EngineerPlayerTemplate;
		break;
	case EPlayerClass::Gunner:
		DefaultPawnClass = GunnerPlayerTemplate;
		break;
	case EPlayerClass::Medic:
		DefaultPawnClass = MedicPlayerTemplate;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("We had a base player class type.  Giving random class"));
		EPlayerClass rand = (EPlayerClass)(FMath::RandRange(0, 3));
		GetPlayerClassFromType(rand);
		return;
		break;
	}
	
	//Remove the class from the list if the list still has it
	if(AvailableClasses.Contains(InClass))
		AvailableClasses.RemoveSingle(InClass);

}

void AStellarFeverMultiplayerGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SetDifficultyValue(GetDifficulyValue() + 1.0f);

	if (ABasePlayer* player = NewPlayer->GetPawn<ABasePlayer>())
	{
		//Switch the inventory to Gun after we have successfully logged in
		
		player->InitializeInventoryForPlay();
	}
}

void AStellarFeverMultiplayerGameMode::Logout(AController* Exiting)
{
	//Since a player logged out we need to decrement the player amount
	--PlayerAmount;

	if (ASF_PlayerController* exitingPlayer = Cast<ASF_PlayerController>(Exiting))
	{
		if (PlayerControllerArray.Contains(exitingPlayer))
		{
			PlayerControllerArray.Remove(exitingPlayer);
		}
	}

	//Make sure that if a player logs out while in the last menu that we notify the end game 
	//That way it updates and lets everyone else move to the Player Select lobby again
	PlayerMadeEndGameChoice(false);

	GEngine->AddOnScreenDebugMessage(0, 5, FColor::Blue, "** A PLAYER HAS SUCCESSFULLY LOGGED OUT **");

	Super::Logout(Exiting);
}

void AStellarFeverMultiplayerGameMode::PlayerMadeEndGameChoice(bool bWantsToContinue)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, "Player wanted to play again - multiplayer");
	if(bWantsToContinue == true)
		iPlayerEndGameChoiceCount++;

	if (iPlayerEndGameChoiceCount >= PlayerAmount)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, "All players wanted to play again, loading");
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		FString PlayerSelectMap = "/Game/Maps/PlayerSelect?listen?Game=/Game/MainMenu/BP_LobbyGamemode.BP_LobbyGamemode_C";

		World->ServerTravel(PlayerSelectMap);
	}
}
