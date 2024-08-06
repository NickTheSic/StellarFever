// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../Player/PlayerInformation.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	TArray<EPlayerClass> SelectedClasses;

	FString GameMap = "ShipWithSystemsV3";
	uint16 uiConnectedPlayers = 0;
	uint16 uiMaxPlayerCheck = 4;

	bool bIsStartingGame = false;

public:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable, Exec)
		void StartGame();

	//Find player of class and tell which playercontroller to possess it.
	void PlayerPossessNewPlayer(APlayerController* Player, const EPlayerClass& playerClass, uint16 playerIndex = 0);

	//Tell the playere controllers which classes have been selected so they can deactivate the correct button on their hud
	void TellAllOfSelectedClasses();

	//Tell the players they can select a player now that the owner has logged out
	void TellAllToReactivateButton(const EPlayerClass& ReactivateClass);

protected:
	virtual void BeginPlay() override;

	TArray<AController*> m_ConnectedPlayers;

	//Tells the pawn to disable visibility and collision so it doesn't get in the way
	void HidePreviousPawn(APlayerController* Player);

	//This passes the player state and class to the game instance so it can add the class to the player map
	void TellInstanceToAddClass(APlayerController* Player, const EPlayerClass& InClass = EPlayerClass::Base);

	
	UPROPERTY(EditAnywhere, Category = "Player Characters")
		TSubclassOf<class ABasePlayer> CaptainPlayerTemplate;

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		TSubclassOf<class ABasePlayer> EngineerPlayerTemplate;

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		TSubclassOf<class ABasePlayer> GunnerPlayerTemplate;

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		TSubclassOf<class ABasePlayer> MedicPlayerTemplate;


	UPROPERTY(EditAnywhere, Category = "Player Characters")
		FVector PlayerStartLocation = FVector();

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		FVector CaptainSpawnLocation = FVector();

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		FVector EngineerSpawnLocation = FVector();

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		FVector GunnerSpawnLocation = FVector();

	UPROPERTY(EditAnywhere, Category = "Player Characters")
		FVector MedicSpawnLocation = FVector();

};
