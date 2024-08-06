#include "VaccineObject.h"
// Copyright Quarantine Games 2020


#include "VaccineObject.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

AVaccineObject::AVaccineObject()
{
	PrimaryActorTick.bCanEverTick = true;

    bIsPickedUpVaccine = false;

	pMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);


    pColliderPickup = CreateDefaultSubobject<UBoxComponent>("Collider Comp");
    pColliderPickup->SetupAttachment(RootComponent);
    pColliderPickup->SetCollisionProfileName("OverlapAll");
    pColliderPickup->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);


	SetReplicates(true);
	SetReplicateMovement(true);

    Tags.Add("Vaccines");
}

void AVaccineObject::BeginPlay()
{
    Super::BeginPlay();
}

void AVaccineObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

bool AVaccineObject::CanBePickedUp()
{
    return (!GetWorldTimerManager().IsTimerActive(TimerHandle_PickUp));
}

void AVaccineObject::Activate()
{
    Super::Activate();
}

void AVaccineObject::Deactivate()
{
    Super::Deactivate();
}

void AVaccineObject::PickUpObject()
{
	{
        Super::PickUpObject();
	    //BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	   // BoxCollider->SetCollisionProfileName("NoCollision");
		//SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//SetCollisionProfileName("NoCollision");
    }
}

void AVaccineObject::DropObject(FVector direction)
{
	{
        Super::DropObject(direction);
	   // BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	   // BoxCollider->SetCollisionProfileName("OverlapAll");
        GetWorldTimerManager().SetTimer(TimerHandle_PickUp, 1.f, false);
    }
}
