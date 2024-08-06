// Copyright Quarantine Games 2020


#include "FuelObject.h"

AFuelObject::AFuelObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("FuelObject");
}

// Called when the game starts or when spawned
void AFuelObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFuelObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFuelObject::Activate()
{
	Super::Activate();
}

void AFuelObject::Deactivate()
{
	Super::Deactivate();
}

void AFuelObject::PickUpObject()
{
	Super::PickUpObject();
}

void AFuelObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}