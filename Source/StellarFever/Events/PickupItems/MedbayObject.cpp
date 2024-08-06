// Copyright Quarantine Games 2020


#include "MedbayObject.h"

AMedbayObject::AMedbayObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("MedbayObject");
}

// Called when the game starts or when spawned
void AMedbayObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMedbayObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMedbayObject::Activate()
{
	Super::Activate();
}

void AMedbayObject::Deactivate()
{
	Super::Deactivate();
}

void AMedbayObject::PickUpObject()
{
	Super::PickUpObject();
}

void AMedbayObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}