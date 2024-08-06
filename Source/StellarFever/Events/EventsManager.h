// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Union.h"
#include "EventsManager.generated.h"


//Has to be exactly 16 bits; a short
USTRUCT()
struct FEventsState
{
    GENERATED_BODY()
    bool oxygen : 1;
    bool lifesupport : 1;
    bool depressure : 1;
    bool lengine : 1;
    bool rengine : 1;
    bool boarding : 1;
    bool enemyship : 1;
    bool teleport : 1;
    bool greenhouse : 1;
    bool bridge : 1;
    bool power : 1;
    bool bathroom : 1;
    bool playershield : 1;

    bool unused : 3;
};

/**
EventsManager is generally used to store and keep track of all events. It will handle activating events when prompted and keeping track of which are
currently active as well. All logic of the events are specific per event so this class will not actively preform any actions on the game, only telling
specific events to be active. 
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class STELLARFEVER_API UEventsManager : public UObject
{
	GENERATED_BODY()

public:

    //All Variables Below

    //Array of all main events assigned in blueprints, such as room specific events and item spawning events
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainEvents")
        TArray<class ABaseEvent*> pMainEvents;

    //Array of all minor events assigned in blueprints, such as doors and general room events that can occur wherever placed in map
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MinorEvents")
        TArray<class ABaseEvent*> pMinorEvents;

    //The Pool used by other events to spawn them and reuse older spawned AI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPool")
        class AEnemyPool* pEnemyPool;

    //A link to roundmanageer to set which event has activated to trigger its audio
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundManager")
        class ARoundManager* pRoundManager;

    //Timer values for when an event can occur, has a min and max for a slight randomization per when an event can occur so it is not every minute exactly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainEventTimer")
        float fRandomMinVal = 40.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainEventTimer")
        float fRandomMaxVal = 60.0f;

    //Timer values for when a minor event can occur, has a min and max for randomness
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MinorEventTimer")
        float fRandomMinorMinVal = 10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MinorEventTimer")
        float fRandomMinorMaxVal = 20.0f;

    float fRandomMainVal;
    FTimerHandle MainEventTimer;

    float fRandomMinorVal;
    FTimerHandle MinorEventTimer;

    //Pointer to the GameMode that holds the global timer
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
        class AStellarFeverGameModeBase* pGameMode;

    //Diffuculty value for the events set up in gamemode
    float fDifficultyValue;

    //The Scale of how easy/hard the game will be
    float fDifficultyScale = 10.0f;

    //The Scale for how easy/hard will be with minor events
    float fDifficultyMinorScale = 4.0f;

    //Timer for triggering an event through a main event
    FTimerHandle TriggerEventTimer;


public:

    //All Functions Below

    //Assigns the Gamemode to the gamemode pointer
    UFUNCTION()
        void SetEventsGameMode(class AStellarFeverGameModeBase* gamemode) { pGameMode = gamemode; }

    //Adds all events to the right array of event types
    UFUNCTION()
        void AddEventsToArray();

    //Works as a basic Init to set all values and is called at the start of all rounds
    UFUNCTION()
        void ResetAllEvents();

    //Sets up the Player pointer in each event
    UFUNCTION()
        void SetUpEvents();

    //Sets up the AIPool to start the spawning of AI
    UFUNCTION()
        void SetUpPools();

    //A function to return the pool for spawning in AI
    UFUNCTION()
        AEnemyPool* GetEnemyPool() { return pEnemyPool; }

    //Called in ResetAllEvents and is also used to restart the timer wil certain events call for it
    UFUNCTION()
        void StartGlobalTimer();

    //Called only by events to specifically stop the round timer
    UFUNCTION()
        void StopGlobalTimer();

    //Called by engine events to properly set if they are active or not to the gamemode
    UFUNCTION()
        void SetEngineIsActive(bool isactive);

    //Activates a random main event, acts seperately from AI activation to ensure that there are consequences for leaving events broken, but onyl a certain amount of
    //random events can occur
    UFUNCTION()
        void ActivateMainEvent();

    //Activates a minor event, ensures not too many minor events caused randomly will be active.
    UFUNCTION()
        void ActivateMinorEvent();

    //Forcefully activates a main event on the map through other events
    UFUNCTION()
        void TriggerMainEvent();

    //Forcefully activates a minor event on the map through other events
    UFUNCTION()
        void TriggerMinorEvent();

    //Ensures that all main events cannot be interacted with except the specific one calling the function
    UFUNCTION()
        void BlockInteraction(class ABaseEvent* safeEvent);

    //Ensures all blocked events can be interacted with again, cycling through the main events turning on the collision again.
    UFUNCTION()
        void AllowInteraction();

    //Returns the value of the Difficulty
    UFUNCTION()
        uint8 GetDifficultyValue() { return fDifficultyValue; }

    //Sets the value of the Difficulty
    UFUNCTION()
        void SetDifficultyValue(float ftempValue) { fDifficultyValue = ftempValue; }

    //Used to set the timers for when an event starts
    UFUNCTION()
        float CreateDifficultyValue();

    //Used to set the timer for the minor events
    UFUNCTION()
        float CreateMinorDifficultyValue();

    //Gets a specific event, pass in the tag the event would have and will return the first specific event of that name
    UFUNCTION()
        ABaseEvent* GetEvent(FName eventName);

    //Checks if all events are active or not, will return true if one is active
    UFUNCTION()
        bool CheckIfEventsAreActive();

    //Stops the Event Timers
    UFUNCTION()
        void StopEventTimers();

    //Starts the event timers using the create difficulty values as time
    UFUNCTION()
        void StartEventTimers();

    UFUNCTION()
        FORCEINLINE FEventsState& GetEventsState() { return *(FEventsState*)&usEventState; }

    //Sets the roundmanager from gamemode
    UFUNCTION()
        void SetRoundManager(ARoundManager* manager) { pRoundManager = manager; }
        
    protected:

    UPROPERTY()
        uint16 usEventState = 0;




};
