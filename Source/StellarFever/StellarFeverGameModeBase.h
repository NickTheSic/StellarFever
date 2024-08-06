// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Miscellaneous/EndGameEnum.h"
#include "StellarFeverGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class STELLARFEVER_API AStellarFeverGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

    AStellarFeverGameModeBase();

    virtual void BeginPlay() override;

    virtual void PostLogin(class APlayerController* NewPlayer) override;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
                    //// STUFF FOR EVENTS ////
public:

    //The Timer for the level; how long it takes for one round to occur
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Time")
        float fRoundTime;

    //Logic to get the EventMAnager Spawning into the gamemode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsManager")
        TSubclassOf<class UEventsManager> EventManagerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventsManager")
        class UEventsManager* pEventManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundManager")
        class ARoundManager* pRoundManager;

    //Ship State pointer for fuel management
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipState")
        class AShipState* pShipState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
        float fDifficultyValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Players")
        uint8 ucPlayerVal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartingFuel")
        float fStartingFuelVal = 1001.0f;

    //A Timer that will occur once every second outside of tick so it specifically can be turned on and off without tick
    FTimerHandle DeductBySecondTimer;

    //A bool to check if the time should decrease or not, so fuel can still be consumed
    bool bIsBridgeActive;

    //A bool to check if the fuel should decrease twice as fast or not
    bool bIsEngineActive;
    bool bIsBothEngineActive;

    //Sets if the engine is active or not, takes into account for both engines
    UFUNCTION()
        void SetEngineIsActive(bool isactive);

    //Gets the roundtime as it will change and pause/unpause if the timer is to be displayed somewhere
    UFUNCTION()
        float GetRoundTime() { return fRoundTime; }

    //Sets the roundtime, used for roundmanager. Do not set mid round, can cause display errors
    UFUNCTION()
        void SetRoundTime(float roundTime) { fRoundTime = roundTime; }

    //A Get for the ship state
    UFUNCTION()
        AShipState* GetShipState() { return pShipState; }
    
    //Function that will reduce the global timer by one second, called by the timer when it is looping
    UFUNCTION()
        void DeductSecond();

    //Will start the timer loop and start reducing the roundtime.
    UFUNCTION()
        void StartRoundTimer();

    //Will stop the timer loop and doesnt change the roundtime.
    UFUNCTION()
        void StopRoundTimer();

    //Gets the difficultyvalue from the gamemode
    UFUNCTION()
        float GetDifficulyValue() { return fDifficultyValue; }

    //Sets the difficultyvaalue
    UFUNCTION()
        void SetDifficultyValue(float difficultyValue) { fDifficultyValue = difficultyValue; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //JUSTIN CODE

    //TArray<AActor*> AllPlayers;
    TArray<AActor*> AllEnemies;

    void WhenPlayersAllDie();

    int PlayerAmount;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
				//// Player Selection Templates ////
    UPROPERTY(EditAnywhere, Category = "Player Select")
    TSubclassOf<class ACaptainPlayer>  CaptainPlayerTemplate;
    UPROPERTY(EditAnywhere, Category = "Player Select")
    TSubclassOf<class AEngineerPlayer> EngineerPlayerTemplate;
    UPROPERTY(EditAnywhere, Category = "Player Select")
    TSubclassOf<class AGunnerPlayer>   GunnerPlayerTemplate;
    UPROPERTY(EditAnywhere, Category = "Player Select")
    TSubclassOf<class AMedicPlayer>    MedicPlayerTemplate;


public:
                //// End of game ////
    void EndGame(const EEndGameReason& EndGameReason = EEndGameReason::DefaultReason);

protected:

    TArray<class ASF_PlayerController*> PlayerControllerArray;

};
