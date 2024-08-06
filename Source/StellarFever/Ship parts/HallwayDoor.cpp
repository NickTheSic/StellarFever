// Copyright Quarantine Games 2020


#include "HallwayDoor.h"

AHallwayDoor::AHallwayDoor()
{
	pDoorCollider1 = CreateDefaultSubobject<UBoxComponent>("Door Collider1");
	pDoorCollider1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pDoorCollider1->SetCollisionProfileName("BlockAll");
	pDoorCollider1->SetupAttachment(RootComponent);

	pDoorCollider2 = CreateDefaultSubobject<UBoxComponent>("Door Collider2");
	pDoorCollider2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pDoorCollider2->SetCollisionProfileName("BlockAll");
	pDoorCollider2->SetupAttachment(RootComponent);

	Tags.Add("Hallway Door");
}