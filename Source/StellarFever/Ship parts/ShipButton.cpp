// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipButton.h"
#include "Components/BoxComponent.h"
#include "ShipDoor.h"


#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
// Sets default values
AShipButton::AShipButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("mesh comp");
	pMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pMeshComponent->SetCollisionProfileName("NoCollision");
	RootComponent = pMeshComponent;

	pBoxCollider = CreateDefaultSubobject<UBoxComponent>("box collider");
	pBoxCollider->SetupAttachment(RootComponent);

	Tags.Add("Button");

}

// Called when the game starts or when spawned
void AShipButton::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShipButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShipButton::Activate()
{
	//if (m_door)
	m_door->OpenDoor();

}

