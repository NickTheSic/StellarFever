// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "TutorialManager.generated.h"

UCLASS()
class STELLARFEVER_API ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialManager();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//For spawning AI
	UPROPERTY(EditAnywhere)
	TArray<class ASpawnPoint*> EnemySpawnPoints;
	//Range
	//Melee
	//Shield
	//Shotgun
	//Exploder
	TArray<class AAIEnemy*> Enemies;

	//For spawning AI after engines come back online
	void SpawnAI();

	//Doors to lock and unlock
	UPROPERTY(EditAnywhere)
	TArray<class AShipDoor*> ShipDoors;
	//1 - Upper Right Door
	//2 - Upper Left Door
	//3 - Lower Right Door
	//4 - Lower Left Door
	//5 - Engine Door
	//6 - Oxygen Door
	//7 - Medbay Door
	//8 - Armory Door

	//Locks all doors
	void LockDoors();
	//Unlocks a given door
	void UnlockDoor(class AShipDoor* door);

	//For initial planet interaction
	void ShutDownEngines();
	bool bEnginesDown;
	bool GetEngineCheck() { return bEnginesDown; }

	//For when engines come back online
	void ShutDownHealthOxygen();
	bool bHPOxygenDown;
	bool GetHPOxygenCheck() { return bHPOxygenDown; }

	//For when players repair LifeSupport and Oxygen
	void UnlockArmory();
	bool bArmoryUnlocked;
	bool GetArmoryCheck() { return bArmoryUnlocked; }
	
	//For when players have finished retrieving their health and ammo
	bool bMedBay, bAmmoBay, bBaysDone;
	void UpdateMedBay() { bMedBay = true; }
	void UpdateAmmoBay() { bAmmoBay = true; }
	bool GetBaysCheck() { return bBaysDone; }


	//Array of sound cues to be played
	UPROPERTY(EditAnywhere)
	TArray<class USoundCue*> SoundCues;
	class UAudioComponent* pAudioComponent;
	void PlayGreeting();
	void PlayGreeting_Repeat() { pAudioComponent->SetSound(SoundCues[1]); pAudioComponent->Play(); }
	void PlayEngineRoom();
	void PlayEngineRoom_Repeat() { pAudioComponent->SetSound(SoundCues[3]); pAudioComponent->Play(); }
	void PlayEnemiesAndSystems();
	void PlayEnemiesAndSystems_Repeat() { pAudioComponent->SetSound(SoundCues[5]); pAudioComponent->Play(); }
	void PlayGetHealthAndAmmo();
	void PlayGetHealthAndAmmo_Repeat() { pAudioComponent->SetSound(SoundCues[7]); pAudioComponent->Play(); }
	void PlayReturnToBridge();
	void PlayReturnToBridge_Repeat() { pAudioComponent->SetSound(SoundCues[9]); pAudioComponent->Play(); }
	void PlayFarewell();
	FTimerHandle TutorialTimer;

	bool bTutorialFinished;

	//To access Health Component
	class ABasePlayer* pPlayer;
	//For limiting player health/oxygen
	float fMinOxygen, fMinHealth;
};
