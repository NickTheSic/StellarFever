// Copyright Quarantine Games 2020


#include "OxygenDrainingActor.h"
#include "StellarFever/Player/BasePlayer.h"

// Sets default values
AOxygenDrainingActor::AOxygenDrainingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOxygenDrainingActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AOxygenDrainingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() < ROLE_Authority)
	{
		//This is to show that even though the Client would change the values 
		//The server still changes them and then replicates it
		for (ABasePlayer* player : pPlayers)
		{
			player->ReplenishOxygen(1.0f * DeltaTime);
		}
		return;
	}
	//if (pPlayers.Num() < 1)
		FindPlayers();

	for (ABasePlayer* player : pPlayers)
	{
		player->DrainOxygen(1.0f * DeltaTime);
	}

}
