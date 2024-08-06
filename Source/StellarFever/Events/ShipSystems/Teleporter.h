// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teleporter.generated.h"

UCLASS()
class STELLARFEVER_API ATeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleporter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BELOW/////

	//The main mesh that the players enter to teleport
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* pMesh;

	//The button for the player to press so they can teleport
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button")
		class AEventObject* pButton;

	//The little warp tube thing that comes down after a teleport
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh", Replicated)
		class USkeletalMeshComponent* pTeleRings;

	//The animation for the teleportation rings
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		class UAnimationAsset* pRingAnim;

	//The collider that will teleport the player
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collider")
		class UBoxComponent* pCollider;

	//Using a spawnpoint to move the player to the proper location
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleport Location")
		class ASpawnPoint* pTeleportPoint;

	//The event that deals with the teleporting
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TeleporterEvent")
		class ATeleporterEvent* pTeleporterEvent;

	//A bool that dictates if its a enemy teleporter or our ships teleporter
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyTeleporter")
		bool bIsEnemyTeleporter;

	//Whether the teleporter can teleport or not
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IsActive")
		bool bisActive;

	//The bool for checking if the button was pressed to allow for teleporting
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TeleportCheck")
		bool bCanTeleport;

	//A bool to check if the button was pressed, used for the tick to set timers
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TeleportCheck")
		bool bButtonPressed;

	//Tiemr to allow the button and anim to reset
	FTimerHandle ResetTimer;

	//Timer to activate the teleport
	FTimerHandle TeleportTimer;

	//Time of the animation and how long for the teleport timer
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TimerTime")
		float fTeleportSpeed;

	//Time for the reset and how fast it can be used again
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ResetTime")
		float fResetSpeed;

public:
	/////ALL FUNCTIONS BELOW/////

	//Activates so the teleporter can work
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Deactivates the teleporter so it can no longer teleport
	UFUNCTION()
		void Deactivate();
	UFUNCTION(Server, Reliable)
		void Server_Deactivate();

	//Plays the animation of teleporting
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Animation();

	//Teleports the given player in the function
	UFUNCTION()
		void Teleport(class ABasePlayer* player);

	//Called when the button is pressed
	UFUNCTION()
		void ActivateTeleport();

	//Called by the timer to reset the button and the bools
	UFUNCTION()
		void Reset();

	//Overlap for when the player goes into the collider
	UFUNCTION()
		void OnColliderOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
};
