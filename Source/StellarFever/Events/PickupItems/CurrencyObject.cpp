// Copyright Quarantine Games 2020


#include "CurrencyObject.h"

ACurrencyObject::ACurrencyObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("CurrencyObject");
}

// Called when the game starts or when spawned
void ACurrencyObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACurrencyObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACurrencyObject::Activate()
{
	Super::Activate();
}

void ACurrencyObject::Deactivate()
{
	Super::Deactivate();
}

void ACurrencyObject::PickUpObject()
{
	Super::PickUpObject();
}

void ACurrencyObject::DropObject(FVector direction)
{
	Super::DropObject(direction);
}