// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../../Events/PickUpItem.h"
#include "JanitorPickUp.generated.h"

class AJanitorAI;
class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;
class UCapsuleComponent;
/**
 * 
 */
UCLASS()
class STELLARFEVER_API AJanitorPickUp : public APickUpItem
{
	GENERATED_BODY()

public:
	
	AJanitorPickUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janitor", Replicated)
	AJanitorAI* pJanitor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", Replicated)
		UStaticMeshComponent* pMesh2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCapsuleComponent* pPickupComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", Replicated)
	UBoxComponent* pCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janitor")
		TSubclassOf<class AJanitorAI> pJanitorClass;

	TArray<AActor*> pOutPlayers;


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Called when created to establish basic functionality
	virtual void Activate() override;
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivate();

	//Called when destroyed when in the proper location
	virtual void Deactivate() override;
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDeactivate();

	//Called when the object is pickedup to bind it to the right socket
	virtual void PickUpObject() override;
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPickupObject();

	//Called when the object is dropped
	virtual void DropObject(FVector direction) override;
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDropObject();



	//Enable/Disable Code
	UFUNCTION()
		void Enable(); //Meant for toilet event
	UFUNCTION()
		void Disable(); //Meant for toilet event

	FTimerHandle TimerHandle_DropItemDelay;

	bool bDisabled; //Meant for toilet event

	//Ask ROB!!!!!!
	bool bIsFixing; //make it so he cannot be distracted? picked up? dunno yet


	
};
