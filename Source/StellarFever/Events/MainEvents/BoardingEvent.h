// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../BaseEvent.h"
#include "BoardingEvent.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ABoardingEvent : public ABaseEvent
{
	GENERATED_BODY()

public:
    /////ALL VARIABLES BELOW/////

    //List of all spawnpoints the AI will generate from
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnpoints")
        TArray<class ASpawnPoint*> pSpawnPoints;

    //List of all spawnpoints the boss will generate from
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnpoints")
        TArray<class ASpawnPoint*> pBossSpawnPoints;

    //List of all event objects linked to the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventObjects", Replicated)
        TArray<class AEventObject*> pEventObjects;

    //The current value of activated objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsValue", Replicated)
        int iActivatedObjects;

    //Timer to spawn their respective AI 
    FTimerHandle SpawnAITimer;

    //Values for Spawning speeds for AI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnSpeed")
        float fSpawnAISpeed;

    //Number of max exploders that spawn per call
    //Set the val from 1-3
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnAmount")
        uint8 ucNumOfExploders;

    //Number of max shields that spawn per call
    //Set the val from 1-3
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnAmount")
        uint8 ucNumOfShields;
    
    //Number of max ranged that spawn per call
    //Set the val from 1-3
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnAmount")
        uint8 ucNumOfRanged; 
    
    //Number of max scavengers that spawn per call
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnAmount")
        uint8 ucNumOfScavengers;

    //Number of max melees that spawn per call
    //Set the val from 1-3
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnAmount")
        uint8 ucNumOfMelee;

    //Number of max shotguns that spawn per call
    //Set the val from 1-3
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnAmount")
        uint8 ucNumOfShotgun;

    //A small bool to spawn the boss or not
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossCheck")
        bool bCanSpawnBoss;

    //The TeleporterEvent that is linked to this event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeleporterEvent")
        class ATeleporterEvent* pTeleporterEvent;


    ///// FOR AUDIO /////

    //Audiomanager for when the event activates
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BoardingAudio")
        UAudioComponent* pBoardingAudio;

    //Soundcue for the music
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BoardingMusic")
        USoundCue* pBoardingMusic;


public:
    /////ALL FUNCTIONS BELOW/////

    ABoardingEvent();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    //Adds specific functionality to Activate and Deactivate events, per event
    UFUNCTION()
        virtual void Activate() override;
        virtual void Server_Activate_Implementation() override;

    UFUNCTION()
        virtual void Deactivate() override;
        virtual void Server_Deactivate_Implementation() override;

    //Used to return when an eventobject is interacted with unique function in each event that has a eventobject
    UFUNCTION()
        virtual void EventObjectComplete() override;
        virtual void Server_EventObjectComplete_Implementation() override;

    //A function used to make the text on the event change per shell brought to the guns
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ChangeText(bool isactive);

    //The overrides for the music
    virtual void Multicast_PlayMusic_Implementation() override;
    virtual void Multicast_StopMusic_Implementation() override;

    //Spawns a bunch of AI at designated spawnpoint locations when activated
    UFUNCTION()
        void SpawnAI();
    UFUNCTION(Server, Reliable)
        void Server_SpawnAI();

    //Spawns the boss AI, called every second time the enemies spawn
    UFUNCTION()
        void SpawnBoss();
    UFUNCTION(Server, Reliable)
        void Server_SpawnBoss();

    //Activates the teleporter event when activated
    UFUNCTION()
        void ActivateTeleporter();
};
