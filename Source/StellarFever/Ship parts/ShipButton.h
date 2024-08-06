// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ShipDoor.h"
#include "ShipButton.generated.h"

UCLASS()
class STELLARFEVER_API AShipButton : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShipButton();

	AActor* pObjectToActivate;

	UPROPERTY(EditAnywhere, Category = "button mesh")
		UStaticMeshComponent* pMeshComponent;

	UPROPERTY(EditAnywhere, Category = "box collider")
		UBoxComponent* pBoxCollider;

	UPROPERTY(EditAnywhere)
	AShipDoor* m_door;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Activate();
	

};
