// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../PickUpItem.h"
#include "VaccineObject.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AVaccineObject : public APickUpItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVaccineObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* pColliderPickup;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTimerHandle TimerHandle_PickUp;

	bool bIsPickedUpVaccine;

	bool CanBePickedUp();

public:
	/////ALL VARIABLES BEELOW/////

public:
	/////ALL FUNCTIONS BELOW/////

	//Called when created to establish basic functionality
	virtual void Activate() override;

	//Called when destroyed when in the proper location
	virtual void Deactivate() override;

	//Called when the object is pickedup to bind it to the right socket
	virtual void PickUpObject() override;

	//Called when the object is dropped
	virtual void DropObject(FVector direction) override;

};
