// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyShip.generated.h"

UCLASS()
class STELLARFEVER_API AEnemyShip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/////ALL VARIABLES BELOW/////

	//The EnemyPool that has references to the AI, called and set by Gamemode
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyPool")
		class AEnemyPool* pEnemyPool;

	//List of SpawnPoints for the Turrets, spawning them at the correct locations on ship
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TurretSpawnPoints")
		TArray<class ASpawnPoint*> pTurretSpawnPoints;

	//List of SpawnPoints for AI to spawn when you enter the ship
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InitialSpawnPoints")
		TArray<ASpawnPoint*> pInitialSpawnPoints;

	//Collider if the players moves closer to engaging with the ship, none will spawn when just emerging on ship
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InitialCollider")
		class UBoxComponent* pInitialCollider;

	//A bool to check if the initial spawn has happened or not
	bool bInitialSpawnCheck;

	//List of SpawnPoints when reaching half way box, Reset when vaccine is reached
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MidSpawnPoints")
		TArray<ASpawnPoint*> pMidSpawnPoints;

	//Collider if the players moves half way into the ship
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MidCollider")
		class UBoxComponent* pMidCollider;

	//A bool to check if the mid spawn happened or not
	bool bMidSpawnCheck;

	//List of SpawnPoints when reaching vaccine
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EndSpawnPoints")
		TArray<ASpawnPoint*> pEndSpawnPoints;

	//Collider if the players moves near where the vaccines will spawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EndCollider")
		class UBoxComponent* pEndCollider;

	//A bool to check if the end spawn happened or not
	bool bEndSpawnCheck;

	/*
	List of subclasses that will spawn the pick up objects
	0 - Vaccine
	1 - Currency
	2 - Ammo
	3 - Health
	4 - Fuel
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ObjectsToSpawn")
		TArray<TSubclassOf<class APickUpItem>> pPickups;

	//The spawnpoints for the other objects around the ship
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ObejectSpawnPoints")
		TArray<ASpawnPoint*> pObjectSpawnPoints;

	//All the items with the spawn points for vaccines
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objects")
		TArray<class APickUpItem*> pVaccines;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ObjectSpawnPoints")
		TArray<ASpawnPoint*> pVaccineSpawnPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objects")
		TArray<class APickUpItem*> pCurrencys;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objects")
		TArray<class APickUpItem*> pAmmos;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objects")
		TArray<class APickUpItem*> pHealths;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objects")
		TArray<class APickUpItem*> pFuels;

	//If the enemy ship is active it is true
	bool bIsActive;

public:
	/////ALL FUNCTIONS BELOW/////

	//Called as the initialization by the teleporter event once it is activated
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Called once the event is turned off AND all players are not on the ship
	UFUNCTION()
		void Deactivate();
	UFUNCTION(Server, Reliable)
		void Server_Deactivate();

	//Used to create a random pick up, will spawn it at the given spawnpoint will randomly create a pick up in the list of pPickups
	UFUNCTION()
		void SpawnPickUp(uint8 Pickuptype, class ASpawnPoint* spawnpoint);

	UFUNCTION()
		void RandomPickUp(uint8 Pickuptype);

	//Spawns enemies immediately upon arriving on the ship
	UFUNCTION()
		void InitialSpawn();

	//Spawns enemies Midway through the ship
	UFUNCTION()
		void MidSpawn();

	//Spawns enemies after reaching the vaccine
	UFUNCTION() 
		void EndSpawn();

	//Collision call to find players colliding with it
	UFUNCTION()
		void OnInitialOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	//Collision call to find players colliding with it
	UFUNCTION()
		void OnMidOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	//Collision call to find players colliding with it
	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
};
