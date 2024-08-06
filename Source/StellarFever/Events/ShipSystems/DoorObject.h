// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorObject.generated.h"

UCLASS()
class STELLARFEVER_API ADoorObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BELOW/////

	//The door collider that tells if someone is in front of the door
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerCollider")
		class UBoxComponent* pPlayerCollider;

	//List of players in the door collider so it doesnt close when open
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Players")
		TArray<class ABasePlayer*> pPlayers;

	//List of all event objects used for the minigame to open the door when it is locked
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "EventObjects")
		TArray<class AEventObject*> pEventObjects;

	//The two meshs of the door that will open and close
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Mesh")
		UStaticMeshComponent* pLeftDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Mesh")
		UStaticMeshComponent* pRightDoor;

	//Bools that declare what status the door is in to determine how it is interacted
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "DoorStatus")
		bool bisBroken;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "DoorStatus")
		bool bisGood;

	/* 
	Int that checks the doors status
	0 - Door is closed and can open
	1 - Door has opened
	2 - Door is open and can close
	3 - Door has closed
	4 = Door is locked
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorStatus")
		uint8 ucDoorStatus;

	//Timer for door to properly close
	FTimerHandle DoorCloseTimer;
	float fDoorCloseTime = 0.5f;

	//Timer for door to open properly
	FTimerHandle DoorOpenTimer;
	float fDoorOpenTime = 0.5f;

public:
	/////ALL FUNCTIONS BELOW/////

	//Will open the door and play the anim sequence of opening it
	UFUNCTION()
		void OpenDoor();

	//Will close the door by setting variables
	UFUNCTION()
		void CloseDoor();

	//When called will reset the door to working condition and automatically open
	UFUNCTION()
		void SetDoorToAutomaticOpen();

	//Sets the door to not open at all and 
	UFUNCTION()
		void SetDoorToBroken();

	//Activates the minigame for the event objects
	UFUNCTION()
		void SetUpMinigame();

	//Called when the minigame fails, will add a timer that will call setup minigame again
	UFUNCTION()
		void Failure();

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


};
