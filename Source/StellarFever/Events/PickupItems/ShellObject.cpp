// Copyright Quarantine Games 2020


#include "ShellObject.h"
#include "EngineUtils.h"

// Sets default values
AShellObject::AShellObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("ShellItem");
}

// Called when the game starts or when spawned
void AShellObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShellObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShellObject::Activate()
{
	Super::Activate();
}

void AShellObject::Deactivate()
{
	Super::Deactivate();
}

void AShellObject::PickUpObject()
{
	Super::PickUpObject();
}

void AShellObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}

