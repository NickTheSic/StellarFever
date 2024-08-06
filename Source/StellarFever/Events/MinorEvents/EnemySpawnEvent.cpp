// Copyright Quarantine Games 2020


#include "EnemySpawnEvent.h"
#include "Net/UnrealNetwork.h"

AEnemySpawnEvent::AEnemySpawnEvent()
{
    Tags.Add("MinorEvent");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void AEnemySpawnEvent::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemySpawnEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemySpawnEvent::Activate()
{
    Super::Activate();
}

void AEnemySpawnEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();

    //Spawns a random AI character, and instantly will deactivate the event again.
    SpawnAI();
    bIsActive = false;
    Deactivate();
}

void AEnemySpawnEvent::Deactivate()
{
    Super::Deactivate();
}

void AEnemySpawnEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
}

void AEnemySpawnEvent::SpawnAI()
{
    Server_SpawnAI();
}

void AEnemySpawnEvent::Server_SpawnAI_Implementation()
{
    uint8 randrange = FMath::RandRange(0, pSpawnPoints.Num() - 1);
    if (pSpawnPoints.IsValidIndex(randrange))
    { 
        pEventsManager->GetEnemyPool()->SpawnRandomEnemy(pSpawnPoints[randrange], false);
    }
}
