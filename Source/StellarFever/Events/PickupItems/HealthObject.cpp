// Copyright Quarantine Games 2020


#include "HealthObject.h"

AHealthObject::AHealthObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("HealthObject");
}

// Called when the game starts or when spawned
void AHealthObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHealthObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthObject::Activate()
{
	Super::Activate();
}

void AHealthObject::Deactivate()
{
	Super::Deactivate();
}

void AHealthObject::PickUpObject()
{
	Super::PickUpObject();
}

void AHealthObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}