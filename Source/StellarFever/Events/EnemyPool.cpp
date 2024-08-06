// Copyright Quarantine Games 2020


#include "EnemyPool.h"
#include "SpawnPoint.h"
#include "../AI/AIEnemy.h"

// Sets default values
AEnemyPool::AEnemyPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("AIPool");
}

// Called when the game starts or when spawned
void AEnemyPool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyPool::SpawnRandomEnemy(ASpawnPoint* spawnpoint, bool bisOnShip)
{
	//Assigns a random number and spawns the appropriate random AI
	uint8 randomval = FMath::RandRange(2, 6);
	AAIEnemy* temppointer = SpawnOneEnemy(randomval, spawnpoint);

	//a check if they are on the enemy ship
	if (bisOnShip == true)
	{
		temppointer->ShipBoardedActivate();
	}
}

AAIEnemy* AEnemyPool::SpawnOneEnemy(uint8 AItype, ASpawnPoint* spawnpoint)
{
	if (spawnpoint == nullptr)
		return nullptr;

	FActorSpawnParameters SpawnParams;

	//Properly Adds or spawns the correct AI of AItype
	switch (AItype) {

			//For Scavengers
	case 1: {
		//Checks if the Array has a deactivated AI to use
		for (int i = 0; i < pScavengerArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pScavengerArray.IsValidIndex(i))
			{
				if (pScavengerArray[i]->GetIsActive() == false)
				{
					pScavengerArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pScavengerArray[i]->Activate();
					return pScavengerArray[i];
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				if (pTempActor)
				{
					pTempActor->Activate();
					pScavengerArray.Add(pTempActor);
				}
				return pTempActor;
			}
		}
	}

			//For Shields
	case 2: {
		//Checks if the Array has a deactivated AI to use
		for (int i = 0; i < pShieldArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pShieldArray.IsValidIndex(i))
			{
				if (pShieldArray[i]->GetIsActive() == false)
				{
					pShieldArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pShieldArray[i]->Activate();
					return pShieldArray[i];
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				if (pTempActor)
				{
					pTempActor->Activate();
					pShieldArray.Add(pTempActor);
				}
				return pTempActor;
			}
		}
	}

			//For Ranged
	case 3: {
		//Checks if the Array has a deactivated AI to use
		for (int i = 0; i < pRangedArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pRangedArray.IsValidIndex(i))
			{
				if (pRangedArray[i]->GetIsActive() == false)
				{
					pRangedArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pRangedArray[i]->Activate();
					return pRangedArray[i];
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				if (pTempActor)
				{
					pTempActor->Activate();
					pRangedArray.Add(pTempActor);
				}
				return pTempActor;
			}
		}
	}

		  //For Exploders
	case 4: {
		//Checks if the Array has a deactivated AI to use
		for (int i = 0; i < pExplodersArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pExplodersArray.IsValidIndex(i))
			{
				if (pExplodersArray[i]->GetIsActive() == false)
				{
					pExplodersArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pExplodersArray[i]->Activate();
					return pExplodersArray[i];
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				if (pTempActor)
				{
					pTempActor->Activate();
					pExplodersArray.Add(pTempActor);
				}
				return pTempActor;
			}
		}
	}

		  //For Melee
	case 5: {
		//Checks if the Array has a deactivated AI to use
		for (int i = 0; i < pMeleeArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pMeleeArray.IsValidIndex(i))
			{
				if (pMeleeArray[i]->GetIsActive() == false)
				{
					pMeleeArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pMeleeArray[i]->Activate();
					return pMeleeArray[i];
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				if (pTempActor)
				{
					pTempActor->Activate();
					pMeleeArray.Add(pTempActor);
				}
				return pTempActor;
			}
		}
	}

		  //For Shotgun
	case 6: {
		//Checks if the Array has a deactivated AI to use
		for (int i = 0; i < pShotgunArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pShotgunArray.IsValidIndex(i))
			{
				if (pShotgunArray[i]->GetIsActive() == false)
				{
					pShotgunArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pShotgunArray[i]->Activate();
					return pShotgunArray[i];
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				if (pTempActor)
				{
					pTempActor->Activate();
					pShotgunArray.Add(pTempActor);
				}
				return pTempActor;
			}
		}
	}
	}
	return nullptr;
}

void AEnemyPool::SpawnMultipleEnemies(uint8 scavengerVal, uint8 shieldVal, uint8 rangedVal, uint8 exploderVal, uint8 meleeVal, uint8 shotgunVal, TArray<class ASpawnPoint*> spawnpoint)
{
	//Setting up the spawnpoint stuff so it doesn't break out of TArray range
	uint8 spawnval = 0;

	//Looping through to spawn the scavengers
	for (int i = 0; i < scavengerVal; i++)
	{
		if (!spawnpoint.IsValidIndex(spawnval))
		{
			spawnval = 0;
		}

		SpawnOneEnemy(1, spawnpoint[spawnval]);
		spawnval++;
	}

	//Looping through to spawn the shields
	for (int i = 0; i < shieldVal; i++)
	{
		if (!spawnpoint.IsValidIndex(spawnval))
		{
			spawnval = 0;
		}

		SpawnOneEnemy(2, spawnpoint[spawnval]);
		spawnval++;
	}

	//Looping through to spawn the ranged
	for (int i = 0; i < rangedVal; i++)
	{
		if (!spawnpoint.IsValidIndex(spawnval))
		{
			spawnval = 0;
		}

		SpawnOneEnemy(3, spawnpoint[spawnval]);
		spawnval++;
	}

	//Looping through to spawn the exploders
	for (int i = 0; i < exploderVal; i++)
	{
		if (!spawnpoint.IsValidIndex(spawnval))
		{
			spawnval = 0;
		}

		SpawnOneEnemy(4, spawnpoint[spawnval]);
		spawnval++;
	}

	//Looping through to spawn the melee
	for (int i = 0; i < meleeVal; i++)
	{
		if (!spawnpoint.IsValidIndex(spawnval))
		{
			spawnval = 0;
		}
		SpawnOneEnemy(5, spawnpoint[spawnval]);
		spawnval++;
	}

	//Looping through to spawn the shotguns
	for (int i = 0; i < shotgunVal; i++)
	{
		if (!spawnpoint.IsValidIndex(spawnval))
		{
			spawnval = 0;
		}

		SpawnOneEnemy(6, spawnpoint[spawnval]);
		spawnval++;
	}
}

void AEnemyPool::SpawnBoss(ASpawnPoint* spawnpoint, uint8 AItype)
{
	FActorSpawnParameters SpawnParams;

	//checks which boss to spawn
	switch (AItype) {

		//For ranged boss
	case 0: {
		//Specific loop to spawn bosses
		for (int i = 0; i < pRangedBossArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pRangedBossArray.IsValidIndex(i))
			{
				if (pRangedBossArray[i]->GetIsActive() == false)
				{
					pRangedBossArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pRangedBossArray[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIBoss[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pRangedBossArray.Add(pTempActor);
				break;
			}
		}
		break;
	}
		  //For melee boss
	case 1: {
		//Specific loop to spawn bosses
		for (int i = 0; i < pMeleeBossArray.Num() + 1; i++)
		{
			//if the value of i reaches the limit of the array, that means no AI are available to activate
			if (pMeleeBossArray.IsValidIndex(i))
			{
				if (pMeleeBossArray[i]->GetIsActive() == false)
				{
					pMeleeBossArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
					pMeleeBossArray[i]->Activate();
					break;
				}
			}
			else
			{
				//Spawns an AI of AItype, defined by the value of which AI is spawned
				AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIBoss[AItype], spawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
				pTempActor->Activate();
				pMeleeBossArray.Add(pTempActor);
				break;
			}
		}
		break;
	}
	}
}

void AEnemyPool::SpawnTurret(ASpawnPoint* spawnpoint)
{
	FActorSpawnParameters SpawnParams;

	//Specific loop to spawn turrets
	for (int i = 0; i < pTurretsArray.Num() + 1; i++)
	{
		//if the value of i reaches the limit of the array, that means no AI are available to activate
		if (pTurretsArray.IsValidIndex(i))
		{
			if (pTurretsArray[i]->GetIsActive() == false)
			{
				pTurretsArray[i]->SetActorLocation(spawnpoint->GetActorLocation());
				pTurretsArray[i]->Activate();
				break;
			}
		}
		else
		{
			//Spawns an AI of AItype, defined by the value of which AI is spawned
			AAIEnemy* pTempActor = GetWorld()->SpawnActor<AAIEnemy>(pAIUnits[0], spawnpoint->GetActorLocation(), spawnpoint->GetActorRotation(), SpawnParams);
			pTempActor->Activate();
			pTurretsArray.Add(pTempActor);
			break;
		}
	}
}

bool AEnemyPool::EnemiesActiveCheck()
{
    return false;
}

