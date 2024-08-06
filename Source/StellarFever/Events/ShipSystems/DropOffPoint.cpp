// Copyright Quarantine Games 2020


#include "DropOffPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../../Bay/AmmoBay.h"
#include "../../Bay/MedicBay.h"
#include "../../StellarFeverGameState.h"
#include "../../Miscellaneous/ShipState.h"
#include "../PickUpItem.h"
#include "../SpawnPoint.h"
#include "../PickupItems/VaccineObject.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADropOffPoint::ADropOffPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting up the collider and assigning as root
	pCollider = CreateDefaultSubobject<UBoxComponent>("Collider");
	pCollider->SetCollisionProfileName("OverlapAllDynamic");
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	RootComponent = pCollider;

	//Setting up the mesh 
	pSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	pSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pSkeletalMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADropOffPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		pCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ADropOffPoint::OnObjectOverlap);
		pCollider->OnComponentBeginOverlap.AddDynamic(this, &ADropOffPoint::OnObjectOverlap);

		//Assigning and finding the ship state
		AStellarFeverGameState* ptempstate = Cast<AStellarFeverGameState>(GetWorld()->GetGameState());
		pShipState = ptempstate->GetShipState();
	}

}

// Called every frame
void ADropOffPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADropOffPoint::AddAmmo()
{
	pAmmoBay->PartialRefill(10);
}

void ADropOffPoint::AddHeals()
{
	pMedBay->PartialRefill(200);
}

void ADropOffPoint::AddCurrency()
{
	pShipState->AddMoney(5);
}

void ADropOffPoint::AddFuel()
{
	pShipState->AddFuelAmount(200.0f);
}

void ADropOffPoint::AddVaccine()
{
	pShipState->AddVaccines(1);

	//Spawning a Vaccine in the Cargo hold on a location
	FActorSpawnParameters SpawnParams;
	AVaccineObject* pTempActor = GetWorld()->SpawnActor<AVaccineObject>(pVaccineclass, pVaccineSpawn->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
}

void ADropOffPoint::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Ensures that the item is a pickup item and sets the cast
	if (OtherActor->ActorHasTag("PickUpItem"))
	{
		APickUpItem* ptemppointer = Cast<APickUpItem>(OtherActor);
		//Whole set of checks to see which item is dropped off into the collider
		if (OtherActor->ActorHasTag("AmmoObject"))
		{
			AddAmmo();
		}
		else if (OtherActor->ActorHasTag("HealthObject"))
		{
			AddHeals();
		}
		else if (OtherActor->ActorHasTag("CurrencyObject"))
		{
			AddCurrency();
		}
		else if (OtherActor->ActorHasTag("FuelObject"))
		{
			AddFuel();
		}
		else if (OtherActor->ActorHasTag("Vaccines"))
		{
			AddVaccine();
		}
		ptemppointer->Deactivate();
	}
}

