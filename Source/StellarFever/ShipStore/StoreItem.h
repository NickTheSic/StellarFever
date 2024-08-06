// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoreItem.generated.h"

UCLASS()
class STELLARFEVER_API AStoreItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStoreItem();

	//Activation and Deactivation for the objects
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Deactivate is used specifically for activating a level/round, when interacted by the player
	UFUNCTION()
		void Disable();
	UFUNCTION(Server, Reliable)
		void Server_Disable();

protected:
	// Called when the game starts or when spawned
	UPROPERTY()
		class AShipState* pshipstate;

	int iAmount = 1; //amount of munition u get
	int iCost = 1; //coin that is worth

	//highest to lowest range where designers can adjust the value of item amount and costs
	UPROPERTY(EditAnywhere, Category = "Ranged Value")
		int iMinAmount = 0;

	UPROPERTY(EditAnywhere, Category = "Ranged Value")
		int iMaxAmount = 1;

	UPROPERTY(EditAnywhere, Category = "Ranged Cost")
		int iMinCost = 0;

	UPROPERTY(EditAnywhere, Category = "Ranged Cost")
		int iMaxCost = 1;

	UPROPERTY(EditAnywhere, Category = "Name of Item")
		FString Name = "";

	//The main mesh for the shop to appear on
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		class USkeletalMeshComponent* pMesh;

	virtual void BeginPlay() override;

public:

	virtual void Purchase();
	void RandomizeAmount();
	void RandomizeCost();
	FString Getname() { return Name; }
	int GetCost(){ return iCost; }
	int GetAmount() { return iAmount; }
	void SetShipState(class AShipState* shipstate) {pshipstate = shipstate;}

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
