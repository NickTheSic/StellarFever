// Fill out your copyright notice in the Description page of Project Settings.


#include "OxygenSupplyActor.h"
#include "EngineUtils.h"

// Sets default values
AOxygenSupplyActor::AOxygenSupplyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("OxygenItem");

}

// Called when the game starts or when spawned
void AOxygenSupplyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOxygenSupplyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOxygenSupplyActor::Activate()
{
	Super::Activate();
}

void AOxygenSupplyActor::Deactivate()
{
	Super::Deactivate();
}

void AOxygenSupplyActor::PickUpObject()
{
	Super::PickUpObject();
}

void AOxygenSupplyActor::DropObject(FVector direction)
{
	Super::DropObject(direction);
}

