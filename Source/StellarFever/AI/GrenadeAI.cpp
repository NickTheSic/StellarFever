// Copyright Quarantine Games 2020


#include "GrenadeAI.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGrenadeAI::AGrenadeAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NetUpdateFrequency = 8; //FRICKIN IMPORTANT!

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AGrenadeAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenadeAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

