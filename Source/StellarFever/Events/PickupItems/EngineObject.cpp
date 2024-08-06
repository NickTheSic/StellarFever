// Copyright Quarantine Games 2020


#include "EngineObject.h"

// Sets default values
AEngineObject::AEngineObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("EngineItem");

}

// Called when the game starts or when spawned
void AEngineObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEngineObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEngineObject::Activate()
{
	Super::Activate();
}

void AEngineObject::Deactivate()
{
	Super::Deactivate();
}

void AEngineObject::PickUpObject()
{
	Super::PickUpObject();
}

void AEngineObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}

