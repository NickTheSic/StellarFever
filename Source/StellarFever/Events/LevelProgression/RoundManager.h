// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoundManager.generated.h"

UCLASS()
class STELLARFEVER_API ARoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
    /////ALL VARIABLES BELOW/////

	//Main Mesh that the players will see and have text appear on
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MainMesh")
		class UStaticMeshComponent* pMesh;

	//The box collider that will detect if the players are near the table
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayerCollider")
		class UBoxComponent* pCollider;

	//Keeps track of how many level were chosen
	UPROPERTY(BlueprintReadWrite, Editanywhere, Category = "Levels", Replicated)
		uint8 ucLevelValue;

	//Text that will display the round timer
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TimeText", Replicated)
		class UTextRenderComponent* pTimerText;

	//The text that will display the fuel remaining , placed off to the side
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FuelText", Replicated)
		UTextRenderComponent* pFuelText;

	//The Difficultyvalue used by the eventmanager and initially set by the gamemode
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DifficultyValue", Replicated)
		float fDifficultyValue;

	//The eventmanager that is used to search for active events
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EventManager")
		class UEventsManager* pEventManager;

	//The gamemode that is used for setting the difficulty
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameMode")
		class AStellarFeverGameModeBase* pGamemode;

	//The bool that checks if the level is active
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "isActive", Replicated)
		bool bIsActive;

	//List of the planets that the player will interact with, contains all data for the level
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Planets", Replicated)
		TArray<class ARoundPlanet*> pPlanets;

	//List of selectable planets that are active to start the next round
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NextRound", Replicated)
		TArray<class ARoundPlanet*> pNextRounds;

	//The number of how many different planets the player may choose from between rounds
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NumOfChoices")
		uint8 ucNumOfChoices;

	//A Bool for the lil planet animation that plays when a player gets near
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayerBool", Replicated)
		bool bPlayerCheck;

	//Tutorial bool check
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TutorialCheck")
		bool bTutorial;

	//Values for the check if event is active and the respective timer
	FTimerHandle EventCheckTimer;
	float fEventCheckTime = 2.5f;

	//A list of players to find their current location whether it is on the ship
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Players")
		TArray<class ABasePlayer*> pPlayers;

	//A link to the shop so we can activate and deactivate it properly
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shop", Replicated)
		class AShipshop* pShop;

	///// AUDIO STUFF /////

	//Audio manager for level select
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MainAudioManager")
		class UAudioComponent* pLevelSelectAudioManager;

	//Audio manager for base track
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BaseAudioManager")
		UAudioComponent* pLevelBaseAudioManager;

	//Sounds
	
	//Level Select Sound
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LevelSelectMusic")
		class USoundCue* pLevelSelectMusic;

	//Level Base Sound
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LevelBaseMusic")
		class USoundCue* pLevelBaseMusic;

	FTimerHandle DurationTimer;

	//Timer for when to activate an event if none are active
	FTimerHandle EventTriggerTimer;

	//The length of the clips
	float fClipDuration = 8.0f;

	//Next event music to play
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NextEventToPlay")
		class ABaseEvent* pNextEvent;

public:
    /////ALL FUNCTIONS BELOW/////

	//Using the values from a planet to activate the proper events and difficulty
	UFUNCTION()
		void StartLevel(ARoundPlanet* planet);
	UFUNCTION(Server, Reliable)
		void Server_StartLevel(ARoundPlanet* planet);

	//Searches all events and will allow for Deactivating
	UFUNCTION()
		void EventsActiveCheck();

	//Grabs a planet from the array from planets and returns it to be used
	UFUNCTION()
		ARoundPlanet* GetRandomPlanet();

	//Deactivates the manager, called when the level is chosen and will start displaying the time left
	UFUNCTION()
		void Deactivate();
	UFUNCTION(Server, Reliable)
		void Server_Deactivate();

	//Activates the manager allowing for choosing of which level to proceed into, will disable all other displays
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Resets the Roundmanager to set variables back to starting values, used as an Init with gamemode
	UFUNCTION()
		void Reset();

	//Changes the level timer whenever it is called in gamemode
	UFUNCTION()
		void SetTimerText(float roundTime);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetTimerText(float roundTime);

	//Changes the fuel text to properly display
	UFUNCTION()
		void SetFuelText(float fuel);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetFuelText(float fuel);

	//Sets the difficultyvalue, used by event manager and applies to all events
	UFUNCTION()
		void SetDifficultyValue(float difficultyValue);

	//Plays a sound that is given to the base audiomanager
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_PlaySound(bool blevelselect);

	//A set to quickly assign the next event so their audio can be played next
	UFUNCTION()
		void SetNextEvent(ABaseEvent* event) { pNextEvent = event; }

	//Plays the given events music when the timer calls it
	UFUNCTION()
		void PlayEventMusic();

	//Adds all players to the array called once the roundmanager activates
	UFUNCTION()
		void AddPlayersToArray();

	//Sets the eventmanager
	UFUNCTION()
		void SetEventManager(UEventsManager* eventManager) { pEventManager = eventManager; }

	//Sets the gamemode
	UFUNCTION()
		void SetGameMode(AStellarFeverGameModeBase* gameMode) { pGamemode = gameMode; }

	//Returns the amount of jumps made
	UFUNCTION()
		uint8 GetNumberOfLevels() { return ucLevelValue; }

	//Returns the shop pointer
	UFUNCTION()
		AShipshop* GetShop() { return pShop; }

	//Checks if any events are active each second to see if there should be on active or not
	UFUNCTION()
		void CheckEvents();

	//Activates an event using the main event thing
	UFUNCTION()
		void ActivateEvent();

	////An init for once the game starts to activate all planets
	//UFUNCTION()
	//	void BeginGame();

	//Collision call to find players colliding with it
	UFUNCTION()
		void OnColliderOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	//Collision to see if a player leaves the collider
	UFUNCTION()
		void OnColliderExit(class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);







	//Matt added this to fix the dissapearing planets
	UPROPERTY(Replicated)
		uint8 ucPlayersTouching = 0;
};
