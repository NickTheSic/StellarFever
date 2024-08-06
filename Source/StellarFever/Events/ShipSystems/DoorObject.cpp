// Copyright Quarantine Games 2020


#include "DoorObject.h"
#include "../../Player/BasePlayer.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
ADoorObject::ADoorObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//the base collider that will detect the player
	pPlayerCollider = CreateDefaultSubobject<UBoxComponent>("PlayerCollider");
	pPlayerCollider->SetCollisionProfileName("OverlapAllDynamic");
	RootComponent = pPlayerCollider;

	//The overlap event when the player enters the collider
	pPlayerCollider->OnComponentBeginOverlap.AddDynamic(this, &ADoorObject::OnColliderOverlap);
	pPlayerCollider->OnComponentEndOverlap.AddDynamic(this, &ADoorObject::OnColliderExit);

	//Sets up both the door colliders
	pLeftDoor = CreateDefaultSubobject<UStaticMeshComponent>("LeftDoorMesh");
	pLeftDoor->SetCollisionProfileName("BlockAll");
	pLeftDoor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pLeftDoor->SetCanEverAffectNavigation(false);
	pLeftDoor->SetupAttachment(RootComponent);

	pRightDoor = CreateDefaultSubobject<UStaticMeshComponent>("RightDoorMesh");
	pRightDoor->SetCollisionProfileName("BlockAll");
	pRightDoor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pRightDoor->SetCanEverAffectNavigation(false);
	pRightDoor->SetupAttachment(RootComponent);

	//Sets up basic bools
	bisGood = true;
	bisBroken = false;
	ucDoorStatus = 0;
}

// Called when the game starts or when spawned
void ADoorObject::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < pEventObjects.Num(); i++)
	{

	}
}

// Called every frame
void ADoorObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoorObject::OpenDoor()
{
	if (bisBroken == false && ucDoorStatus == 0)
	{
		ucDoorStatus = 1;
	}
	if (ucDoorStatus == 3)
	{
		GetWorld()->GetTimerManager().SetTimer(DoorOpenTimer, this, &ADoorObject::OpenDoor, fDoorOpenTime);
	}
}

void ADoorObject::CloseDoor()
{
	if (ucDoorStatus == 2)
	{
		ucDoorStatus = 3;
	}
	if (ucDoorStatus == 1)
	{
		GetWorld()->GetTimerManager().SetTimer(DoorCloseTimer, this, &ADoorObject::CloseDoor, fDoorCloseTime);
		ucDoorStatus = 2;
	}
	if (bisBroken == true)
	{
		ucDoorStatus = 4;
	}
}

void ADoorObject::SetDoorToAutomaticOpen()
{	
	//Sets the door to open automatically when in the collider
	ucDoorStatus = 0;
	bisGood = true;
	bisBroken = false;
}

void ADoorObject::SetDoorToBroken()
{
	//Sets the door to be broken, however if already manual will stay broken
	bisBroken = true;
	bisGood = false;
	CloseDoor();
	SetUpMinigame();
}

void ADoorObject::SetUpMinigame()
{

}

void ADoorObject::Failure()
{

}

void ADoorObject::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Calls the player to open the door
	if (OtherActor->ActorHasTag("Player"))
	{
		ABasePlayer* pTempPlayer = Cast<ABasePlayer>(OtherActor);
		pPlayers.Add(pTempPlayer);
		if (bisGood == true)
		{
			OpenDoor();
		}
	}

	if (OtherActor->ActorHasTag("Enemy"))
	{
		if (bisBroken == false)
		{
			OpenDoor();
		}
	}
}

void ADoorObject::OnColliderExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//cycles through to find the matching player and deletes them out of the array
	if (OtherActor->ActorHasTag("Player"))
	{
		ABasePlayer* pTempPlayer = Cast<ABasePlayer>(OtherActor);

		for (int i = 0; i < pPlayers.Num(); i++)
		{
			if (pTempPlayer == pPlayers[i])
			{
				pPlayers.RemoveAt(i);
			}
		}
	}

	if (pPlayers.Num() == 0)
	{
		CloseDoor();
	}

	if (OtherActor->ActorHasTag("Enemy"))
	{
		if (bisBroken == false && pPlayers.Num() == 0)
		{
			CloseDoor();
		}
	}
}
