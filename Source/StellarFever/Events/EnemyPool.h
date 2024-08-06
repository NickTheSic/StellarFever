// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPool.generated.h"

UCLASS()
class STELLARFEVER_API AEnemyPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BELOW/////

	//The event manager that contains this pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Manager")
		class UEventsManager* pEventsManager;

		/*Reference to the AI players that will spawn
	0 = Turret
	1 = Scavengers
	2 = Shields
	3 = Ranged
	4 = Exploders
	5 = Melee
	6 = Shotgun
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnedAI")
		TArray<TSubclassOf<class AAIEnemy>> pAIUnits;

	/*Seperates the boss from normal AI that spawns on a different counter
	0 - Ranged Boss
	1 - Melee Boss
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnedAI")
		TArray<TSubclassOf<AAIEnemy>> pAIBoss;

	//All Arrays of the pool themselves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pTurretsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pExplodersArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pShieldArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pRangedArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pScavengerArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pMeleeArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pShotgunArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pRangedBossArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIArrays")
		TArray<AAIEnemy*> pMeleeBossArray;

public:
	/////ALL FUNCTIONS BELOW/////

	//Sets up the eventsmanager for creation
	UFUNCTION()
		void SetEventsManager(class UEventsManager* manager) { if (GetLocalRole() == ROLE_Authority) { pEventsManager = manager; } }

	//Different ways to spawn enemies

	/* Spawns one random enemy, used for making an AI spawn and not worrying which one it is
	*/
	UFUNCTION()
		void SpawnRandomEnemy(class ASpawnPoint* spawnpoint, bool bisOnShip);

	/*Spawns one enemy of given type, passing in a value will spawn that specific enemy, last variable is the spawnpoint given to the function
	1 - Scavenger
	2 - Shield
	3 - Ranged
	4 - Exploder
	5 - Melee
	6 - Shotgun
	*/
	UFUNCTION()
		AAIEnemy* SpawnOneEnemy(uint8 AItype, class ASpawnPoint* spawnpoint);

	/* Spawns all enemies, asks for a value for each type of enemy, last variable is the spawnpoints given to the function
	First - Scavenger
	Second - Shield
	Third - Ranged
	Forth - Exploder
	Fifth - Melee
	Sixth - Shotgun
	*/
	UFUNCTION()
		void SpawnMultipleEnemies(uint8 scavengerVal, uint8 shieldVal, uint8 rangedVal, uint8 exploderVal, uint8 meleeVal, uint8 shotgunVal, TArray<class ASpawnPoint*> spawnpoint);

	//Spawns the Boss, has a limit of one at any time, second variable deals with which boss to spawn
	UFUNCTION()
		void SpawnBoss(class ASpawnPoint* spawnpoint, uint8 AItype);

	//Spawns a Turret at given spawnpoint
	UFUNCTION()
		void SpawnTurret(class ASpawnPoint* spawnpoint);

	//returns if there are enemy activate
	UFUNCTION()
		bool EnemiesActiveCheck();
};
