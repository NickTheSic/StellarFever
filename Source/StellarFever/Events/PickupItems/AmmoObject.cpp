// Copyright Quarantine Games 2020


#include "AmmoObject.h"

AAmmoObject::AAmmoObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("AmmoObject");
}

// Called when the game starts or when spawned
void AAmmoObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAmmoObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoObject::Activate()
{
	Super::Activate();
}

void AAmmoObject::Deactivate()
{
	Super::Deactivate();
}

void AAmmoObject::PickUpObject()
{
	Super::PickUpObject();
}

void AAmmoObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}