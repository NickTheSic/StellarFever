// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropOffPoint.generated.h"

UCLASS()
class STELLARFEVER_API ADropOffPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADropOffPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BELOW/////

	//A Link to the ammo bay so we can refill it
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AmmoBay")
		class AAmmoBay* pAmmoBay;

	//A link to the Medbay so we can refill it
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MedBay")
		class AMedicBay* pMedBay;

	//A link to the Ship state so that we can alter the fuel, currency and vaccine values
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shipstate")
		class AShipState* pShipState;

	//The mesh that where the objects must be brought to
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkeletalMesh")
		class USkeletalMeshComponent* pSkeletalMesh;

	//The collider where the objects will overlap with to call the proper functions
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collider")
		class UBoxComponent* pCollider;

	//The spawnpoint that will place the vaccine in the cargo hold
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnPoint")
		class ASpawnPoint* pVaccineSpawn;

	//The spawning sub class for the vaccine
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Vaccine")
		TSubclassOf<class AVaccineObject> pVaccineclass;

public:
	/////ALL FUNCTIONS BELOW/////

	//The function that will refill the ammo
	UFUNCTION()
		void AddAmmo();

	//The function that will refill the healing
	UFUNCTION()
		void AddHeals();

	//The function that will add to the currency
	UFUNCTION()
		void AddCurrency();

	//The function that will add to the fuel
	UFUNCTION()
		void AddFuel();

	//The function that will add to the vaccines and spawn another in the cargo hold
	UFUNCTION()
		void AddVaccine();

	//Collision call to find players colliding with it
	UFUNCTION()
		void OnObjectOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

};
