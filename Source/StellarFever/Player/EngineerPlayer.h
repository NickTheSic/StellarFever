// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "BasePlayer.h"
#include "EngineerPlayer.generated.h"

/**
 * This is the outline for the Engineer player
 * We inherit from the base player to have a single networked class
 * We override the UseSpecial() function and add the needed code to make it work here
 */

UCLASS()
class STELLARFEVER_API AEngineerPlayer : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	AEngineerPlayer();

	bool UseSpecial() override;

	UFUNCTION(Server, Reliable)
		void Server_FindAndStunEnemies();

	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_SuccesfulStunCallout();

	UPROPERTY(EditAnywhere, Category = "Player | Player Config")
		float fTargetStunRadius = 500.0f;
};
