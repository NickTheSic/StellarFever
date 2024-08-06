// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "EnemySpawnEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AEnemySpawnEvent : public ABaseEvent
{
	GENERATED_BODY()
	
public:
    /////ALL VARIABLES BELOW/////

    //SpawnPoint array to randomly spawn an enemy, set the spawnpoints in blueprints
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnPoints")
        TArray<class ASpawnPoint*> pSpawnPoints;

public:
    /////ALL FUNCTIONS BELOW/////

    AEnemySpawnEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
        virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
        virtual void Server_Deactivate_Implementation() override;

    //Spawns a bunch of AI at designated spawnpoint locations when activated
    UFUNCTION()
        void SpawnAI();
    UFUNCTION(Server, Reliable)
        void Server_SpawnAI();

};
