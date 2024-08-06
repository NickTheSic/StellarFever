// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "ShipDoor.h"
#include "HallwayDoor.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AHallwayDoor : public AShipDoor
{
	GENERATED_BODY()

		AHallwayDoor();

public:
	UPROPERTY(EditAnywhere, Category = "door collider")
		UBoxComponent* pDoorCollider1;

	UPROPERTY(EditAnywhere, Category = "door collider")
		UBoxComponent* pDoorCollider2;
};
