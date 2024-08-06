// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "TeleporterEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ATeleporterEvent : public ABaseEvent
{
	GENERATED_BODY()
	
public:
    /////ALL VARIABLES BELOW/////

    //The teleporter that will be turned on and off
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleporter")
        class ATeleporter* pTeleporter;

    //The teleporter that brings players back to the ship
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleporter")
        ATeleporter* pEnemyTeleporter;

    //The link to the enemyship object that deals with everything on the other ship
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyShip")
        class AEnemyShip* pEnemyShip;

    //The references to the players on the enemy ship
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayersOnShip")
        TArray<class ABasePlayer*> pPlayersOnShip;

    //Bool for if the players are on the enemy ship
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayersOnShip")
        bool bAreOnShip;

public:
    /////ALL FUNCTIONS BELOW/////

    ATeleporterEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
    virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
    virtual void Server_Deactivate_Implementation() override;

    //Adds a player to the array of players on the ship
    UFUNCTION()
        void AddPlayer(ABasePlayer* player);

    UFUNCTION()
        void RemovePlayer(ABasePlayer* player);
};
