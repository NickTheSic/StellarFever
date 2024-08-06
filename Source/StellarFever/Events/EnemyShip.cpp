// Copyright Quarantine Games 2020


#include "EnemyShip.h"
#include "PickUpItem.h"
#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "EnemyPool.h"
#include "../StellarFeverGameModeBase.h"
#include "EventsManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEnemyShip::AEnemyShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting up all colliders to be created and set up, using the initial as the root
	pInitialCollider = CreateDefaultSubobject<UBoxComponent>("InitialCollider");
	pInitialCollider->SetCollisionProfileName("OverlapAllDynamic");
	pInitialCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pInitialCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	RootComponent = pInitialCollider;

	pMidCollider = CreateDefaultSubobject<UBoxComponent>("MidCollider");
	pMidCollider->SetCollisionProfileName("OverlapAllDynamic");
	pMidCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pMidCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pMidCollider->SetupAttachment(RootComponent);

	pEndCollider = CreateDefaultSubobject<UBoxComponent>("EndCollider");
	pEndCollider->SetCollisionProfileName("OverlapAllDynamic");
	pEndCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pEndCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pEndCollider->SetupAttachment(RootComponent);

	SetReplicates(true);

	//Assigning all bools to false
	bIsActive = false;
	bInitialSpawnCheck = false;
	bMidSpawnCheck = false;
	bEndSpawnCheck = false;

}

// Called when the game starts or when spawned
void AEnemyShip::BeginPlay()
{
	Super::BeginPlay();

	//Setting up the colliders delegate calls
	if (GetLocalRole() == ROLE_Authority)
	{
		pInitialCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyShip::OnInitialOverlap);
		pInitialCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShip::OnInitialOverlap);

		pMidCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyShip::OnMidOverlap);
		pMidCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShip::OnMidOverlap);

		pEndCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyShip::OnEndOverlap);
		pEndCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShip::OnEndOverlap);

	}
	
}

// Called every frame
void AEnemyShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyShip::Activate()
{
	Server_Activate();
}

void AEnemyShip::Server_Activate_Implementation()
{
	//Disable all bools in the class
	bInitialSpawnCheck = false;
	bMidSpawnCheck = false;
	bEndSpawnCheck = false;

	//Activate the right active bool
	bIsActive = true;

	//Spawning a vaccine in a random spawn point location to spice some stuff up sometimes
	uint8 randval = FMath::RandRange(0, pVaccineSpawnPoints.Num() - 1);
	SpawnPickUp(0, pVaccineSpawnPoints[randval]);

	//Spawns a fuel object somewhere randomly on the ship
	randval = FMath::RandRange(0, pObjectSpawnPoints.Num() - 1);
	SpawnPickUp(0, pObjectSpawnPoints[randval]);

	//Spawning in a random chance of a pick up for each type
	for (int i = 1; i < 4; i++)
	{
		RandomPickUp(i);
	}

	//Setting the Enemypool
	AStellarFeverGameModeBase* tempPointer = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	pEnemyPool = tempPointer->pEventManager->GetEnemyPool();

	//Spawning the turrets onto the ship
	/*for (int i = 0; i < pTurretSpawnPoints.Num(); i++)
	{
		if (pTurretSpawnPoints.IsValidIndex(i))
		{
			pEnemyPool->SpawnTurret(pTurretSpawnPoints[i]);
		}
	}*/
}

void AEnemyShip::Deactivate()
{
	Server_Deactivate();
}

void AEnemyShip::Server_Deactivate_Implementation()
{
	//Activate the right active bool
	bIsActive = false;

	//Removes all potentially spawned items
	//Vaccines
	for (int i = 0; i < pVaccines.Num(); i++)
	{
		if (pVaccines.IsValidIndex(i))
		{
			if (pVaccines[i]->GetIsActive() == true)
			{
				pVaccines[i]->Deactivate();
			}
		}
	}

	//Currency
	for (int i = 0; i < pCurrencys.Num(); i++)
	{
		if (pCurrencys.IsValidIndex(i))
		{
			if (pCurrencys[i]->GetIsActive() == true)
			{
				pCurrencys[i]->Deactivate();
			}
		}
	}

	//Ammo
	for (int i = 0; i < pAmmos.Num(); i++)
	{
		if (pAmmos.IsValidIndex(i))
		{
			if (pAmmos[i]->GetIsActive() == true)
			{
				pAmmos[i]->Deactivate();
			}
		}
	}

	//Health
	for (int i = 0; i < pHealths.Num(); i++)
	{
		if (pHealths.IsValidIndex(i))
		{
			if (pHealths[i]->GetIsActive() == true)
			{
				pHealths[i]->Deactivate();
			}
		}
	}

	//Fuel
	for (int i = 0; i < pFuels.Num(); i++)
	{
		if (pFuels.IsValidIndex(i))
		{
			if (pFuels[i]->GetIsActive() == true)
			{
				pFuels[i]->Deactivate();
			}
		}
	}
}

void AEnemyShip::SpawnPickUp(uint8 Pickuptype, class ASpawnPoint* spawnpoint)
{
	FActorSpawnParameters SpawnParams;

	//switch case that searches the given value and spawns the correct pick up
	switch (Pickuptype) {

		//For Vaccine Pickups
	case 0: {
		//Checks if the Array has a deactivated pickup to use
		for (int i = 0; i < pVaccines.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no Pickups are available to activate
			if (pVaccines.IsValidIndex(i))
			{
				if (pVaccines[i]->GetIsActive() == false)
				{
					pVaccines[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pVaccines[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns a pickup, defined by the value of which pickup is spawned
				APickUpItem* pTempActor = GetWorld()->SpawnActor<APickUpItem>(pPickups[Pickuptype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pVaccines.Add(pTempActor);
				break;
			}
		}
		break;
	}
		//For Currency Pickups
	case 1: {
		//Checks if the Array has a deactivated pickup to use
		for (int i = 0; i < pCurrencys.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no Pickups are available to activate
			if (pCurrencys.IsValidIndex(i))
			{
				if (pCurrencys[i]->GetIsActive() == false)
				{
					pCurrencys[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pCurrencys[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns a pickup, defined by the value of which pickup is spawned
				APickUpItem* pTempActor = GetWorld()->SpawnActor<APickUpItem>(pPickups[Pickuptype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pCurrencys.Add(pTempActor);
				break;
			}
		}
		break;
	}

		  //For Ammo Refill Pickups
	case 2: {
		//Checks if the Array has a deactivated pickup to use
		for (int i = 0; i < pAmmos.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no Pickups are available to activate
			if (pAmmos.IsValidIndex(i))
			{
				if (pAmmos[i]->GetIsActive() == false)
				{
					pAmmos[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pAmmos[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns a pickup, defined by the value of which pickup is spawned
				APickUpItem* pTempActor = GetWorld()->SpawnActor<APickUpItem>(pPickups[Pickuptype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pAmmos.Add(pTempActor);
				break;
			}
		}
		break;
	}

		  //For Health Refill Pickups
	case 3: {
		//Checks if the Array has a deactivated pickup to use
		for (int i = 0; i < pHealths.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no Pickups are available to activate
			if (pHealths.IsValidIndex(i))
			{
				if (pHealths[i]->GetIsActive() == false)
				{
					pHealths[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pHealths[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns a pickup, defined by the value of which pickup is spawned
				APickUpItem* pTempActor = GetWorld()->SpawnActor<APickUpItem>(pPickups[Pickuptype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pHealths.Add(pTempActor);
				break;
			}
		}
		break;
	}

		  //For Fuel Refill Pickups
	case 4: {
		//Checks if the Array has a deactivated pickup to use
		for (int i = 0; i < pFuels.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no Pickups are available to activate
			if (pFuels.IsValidIndex(i))
			{
				if (pFuels[i]->GetIsActive() == false)
				{
					pFuels[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pFuels[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns a pickup, defined by the value of which pickup is spawned
				APickUpItem* pTempActor = GetWorld()->SpawnActor<APickUpItem>(pPickups[Pickuptype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pFuels.Add(pTempActor);
				break;
			}
		}
		break;
	}
	}
}

void AEnemyShip::RandomPickUp(uint8 Pickuptype)
{
	//Finds a random spawnpoint and gives a current 50/50 chance to spawn the given pickup
	uint8 randspawnpoint = FMath::RandRange(0, pObjectSpawnPoints.Num() - 1);
	uint8 randspawnchance = FMath::RandRange(0, 1);
	if (randspawnchance == 1)
	{
		SpawnPickUp(Pickuptype, pObjectSpawnPoints[randspawnpoint]);
	}
}

void AEnemyShip::InitialSpawn()
{
	//Spawning a set of random enemies onto the ship, using the specific spawn point near the teleporter
	for (int i = 0; i < pInitialSpawnPoints.Num() - 1; i++)
	{
		if (pInitialSpawnPoints.IsValidIndex(i))
		{
			pEnemyPool->SpawnRandomEnemy(pInitialSpawnPoints[i], true);
		}
	}
}

void AEnemyShip::MidSpawn()
{
	//Spawning a set of random enemies onto the ship, using the specific spawn points around in the ship
	for (int i = 0; i < pMidSpawnPoints.Num() - 1; i++)
	{
		if (pMidSpawnPoints.IsValidIndex(i))
		{
			pEnemyPool->SpawnRandomEnemy(pMidSpawnPoints[i], true);
		}
	}
}

void AEnemyShip::EndSpawn()
{
	//Spawning a set of random enemies onto the ship, ensuring a boss spawns and keeping enemies spawn near the vaccines
	for (int i = 0; i < pEndSpawnPoints.Num() - 1; i++)
	{
		if (pEndSpawnPoints.IsValidIndex(i))
		{
			if (i == 0)
			{
				//sets up a random boss to spawn
				uint8 randval = FMath::RandRange(0, 1);
				pEnemyPool->SpawnBoss(pEndSpawnPoints[i], randval);
			}
			else
			{
				pEnemyPool->SpawnRandomEnemy(pEndSpawnPoints[i], true);
			}
		}
	}
}

void AEnemyShip::OnInitialOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If a player walks over the overlap, it will spawn the initial wave of enemies
	if (OtherActor->ActorHasTag("Player"))
	{
		if (bInitialSpawnCheck == false)
		{
			bInitialSpawnCheck = true;
			InitialSpawn();
		}
	}
}

void AEnemyShip::OnMidOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If a player walks over the overlap, it will spawn the mid wave of enemies, this will work on way in and out of the ship
	if (OtherActor->ActorHasTag("Player"))
	{
		if (bMidSpawnCheck == false)
		{
			bMidSpawnCheck = true;
			MidSpawn();
		}
	}
}

void AEnemyShip::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If a player walks over the overlap, it will spawn a boss and a few enemies when near the vaccines
	if (OtherActor->ActorHasTag("Player"))
	{
		if (bEndSpawnCheck == false)
		{
			bEndSpawnCheck = true;
			bMidSpawnCheck = false;
			EndSpawn();
		}
	}
}

