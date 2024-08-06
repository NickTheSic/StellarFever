// Copyright Quarantine Games 2020


#include "BatteryObject.h"
#include "EngineUtils.h"

// Sets default values
ABatteryObject::ABatteryObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("BatteryItem");

}

// Called when the game starts or when spawned
void ABatteryObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABatteryObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABatteryObject::Activate()
{
	Super::Activate();
}

void ABatteryObject::Deactivate()
{
	Super::Deactivate();
}

void ABatteryObject::PickUpObject()
{
	Super::PickUpObject();
}

void ABatteryObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}

